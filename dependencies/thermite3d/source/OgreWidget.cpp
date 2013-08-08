#include "OgreWidget.h"

#include "Application.h"

#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

#include <QSettings>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

namespace Thermite
{
	OgreWidget::OgreWidget(QWidget* parent, Qt::WindowFlags f)
	:ViewWidget(parent, f | Qt::MSWindowsOwnDC)
	,m_pOgreRenderWindow(0)
	,mIsInitialised(false)
	,widthBeforeFullscreen(100)
	,heightBeforeFullscreen(100)
	,xPosBeforeFullscreen(50)
	,yPosBeforeFullscreen(50)
	,mIsFullscreenMode(false)
	{		
		QPalette colourPalette = palette();
		colourPalette.setColor(QPalette::Active, QPalette::WindowText, Qt::black);
		colourPalette.setColor(QPalette::Active, QPalette::Window, Qt::black);
		setPalette(colourPalette);
	}

	OgreWidget::~OgreWidget()
	{
		mBufferFlush.stop();
	}

	bool OgreWidget::initialiseOgre(const Ogre::NameValuePairList *miscParams)
	{
		//These attributes are the same as those use in a QGLWidget
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NoSystemBackground);

		//Parameters to pass to Ogre::Root::createRenderWindow()
		Ogre::NameValuePairList params;
		//params["useNVPerfHUD"] = "true";
		params["gamma"] = "false";
		params["FSAA"] = "4";
		params["vsync"] = "true";

		//If the user passed in any parameters then be sure to copy them into our own parameter set.
		//NOTE: Many of the parameters the user can supply (left, top, border, etc) will be ignored
		//as they are overridden by Qt. Some are still useful (such as FSAA).
		if(miscParams != 0)
		{
			params.insert(miscParams->begin(), miscParams->end());
		}

		//The external windows handle parameters are platform-specific
		Ogre::String externalWindowHandleParams;

	#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
		//positive integer for W32 (HWND handle) - According to Ogre Docs
		externalWindowHandleParams = Ogre::StringConverter::toString((unsigned int)(winId()));
	#endif

	#if defined(Q_WS_X11)
		//poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*) for GLX - According to Ogre Docs
		QX11Info info = x11Info();
		externalWindowHandleParams  = Ogre::StringConverter::toString((unsigned long)(info.display()));
		externalWindowHandleParams += ":";
		externalWindowHandleParams += Ogre::StringConverter::toString((unsigned int)(info.screen()));
		externalWindowHandleParams += ":";
		externalWindowHandleParams += Ogre::StringConverter::toString((unsigned long)(winId()));
		//externalWindowHandleParams += ":";
		//externalWindowHandleParams += Ogre::StringConverter::toString((unsigned long)(info.visual()));
	#endif

		//Add the external window handle parameters to the existing params set.
	#if defined(Q_WS_WIN) || defined(Q_WS_MAC)		
		params["externalWindowHandle"] = externalWindowHandleParams;
	#endif

	#if defined(Q_WS_X11)
		params["parentWindowHandle"] = externalWindowHandleParams;
	#endif

	#if defined(Q_WS_MAC)
		params["macAPI"] = "cocoa";
		params["macAPICocoaUseNSView"] = "true";
	#endif 

		bool bSupportsFSAA2X = false;
		bool bSupportsFSAA4X = false;
		Ogre::RenderSystem* renderSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();
		Ogre::ConfigOption fsaaOptions = renderSystem->getConfigOptions()["FSAA"];		
		for(int ct = 0; ct < fsaaOptions.possibleValues.size(); ct++)
		{
			std::string value = fsaaOptions.possibleValues[ct];
			if(value.at(0) == '2')
			{
				bSupportsFSAA2X = true;
				qDebug() << "Render system supports FSAA 2X";
			}
			if(value.at(0) == '4')
			{
				bSupportsFSAA2X = true;
				qDebug() << "Render system supports FSAA 4X";
			}
		}

		if(bSupportsFSAA4X)
		{
			params["FSAA"] = "4";
			qDebug() << "Using FSAA 4X";
		}
		else if(bSupportsFSAA2X)
		{
			params["FSAA"] = "2";
			qDebug() << "Using FSAA 4X";
		}
		else
		{
			params["FSAA"] = "0";
			qDebug() << "Not using FSAA";
		}


		//Finally create our window.
		try
		{
			qDebug() << "Creating RenderWindow...";
			m_pOgreRenderWindow = Ogre::Root::getSingletonPtr()->createRenderWindow("OgreWindow", width(), height(), false, &params);
			qDebug() << "Success";
		}
		catch(Ogre::Exception& e)
		{
			m_pOgreRenderWindow = 0; //Probably zero anyway, but just to make sure.
			qDebug() << "Failed";
		}

		//Prevent the CPU getting too far ahead.
		//http://www.ogre3d.org/tikiwiki/FlushGPUBuffer
		//http://www.ogre3d.org/forums/viewtopic.php?f=5&t=50486
		mBufferFlush.start(2);

		if(m_pOgreRenderWindow)
		{
			mIsInitialised = true;
		}

		return mIsInitialised;
	}

	Ogre::RenderWindow* OgreWidget::getOgreRenderWindow() const
	{
		return m_pOgreRenderWindow;
	}

	QPaintEngine *OgreWidget:: paintEngine() const
	{
		return 0;
	}

	void OgreWidget::paintEvent(QPaintEvent* evt)
	{
		if(mIsInitialised)
		{
			Ogre::Root::getSingleton()._fireFrameStarted();
			m_pOgreRenderWindow->update();
			Ogre::Root::getSingleton()._fireFrameRenderingQueued();
			Ogre::Root::getSingleton()._fireFrameEnded();
		}
	}

	void OgreWidget::resizeEvent(QResizeEvent* evt)
	{
		qDebug() << "Window resized to " << width() << "x" << height();
		if(m_pOgreRenderWindow)
		{
			m_pOgreRenderWindow->resize(width(), height());
			m_pOgreRenderWindow->windowMovedOrResized();

			for(int ct = 0; ct < m_pOgreRenderWindow->getNumViewports(); ++ct)
			{
				Ogre::Viewport* pViewport = m_pOgreRenderWindow->getViewport(ct);
				Ogre::Camera* pCamera = pViewport->getCamera();
				pCamera->setAspectRatio(static_cast<Ogre::Real>(pViewport->getActualWidth()) / static_cast<Ogre::Real>(pViewport->getActualHeight()));
			}
		}
	}

	void OgreWidget::changeWindowSetup(bool fullscreen)
	{
		if(fullscreen == mIsFullscreenMode)
		{
			//Early out if we are alrady in the correct mode.
			return;
		}

		//We use the Ogre fullscreen methods because the Qt ones don't hide the task bar properly.
		if(fullscreen)
		{
			widthBeforeFullscreen = this->width();
			heightBeforeFullscreen = this->height();
			xPosBeforeFullscreen = this->frameGeometry().left();
			yPosBeforeFullscreen = this->frameGeometry().top();
			move(50,50); //Force to be on the primary monitor, as we've seen hangs when fullscreening on the secondary monitor.
			setWindowState(windowState() | Qt::WindowFullScreen);
			m_pOgreRenderWindow->setFullscreen(true, this->width(), this->height());
			mIsFullscreenMode = true;
		}
		else
		{
			
			m_pOgreRenderWindow->setFullscreen(false, widthBeforeFullscreen, heightBeforeFullscreen);
			move(xPosBeforeFullscreen, yPosBeforeFullscreen);
			setWindowState(windowState() & ~Qt::WindowFullScreen);
			mIsFullscreenMode = false;
		}
	}

	/*bool OgreWidget::applySettings(QSettings* settings)
	{
		bool applied = false;
		QString resolution;

		QStringList windowModes = settings->value("Graphics/WindowModes").toStringList();
		int selectedWindowMode = settings->value("Graphics/SelectedWindowMode", 0).toInt();

		if((windowModes.size() > selectedWindowMode) && (selectedWindowMode != -1)) //Make sure it's a valid index.
		{
			resolution = windowModes.at(selectedWindowMode);
			if(resolution.compare("FullScreen", Qt::CaseInsensitive) == 0)
			{
				//Use next line instead of showFullScreen (http://techbase.kde.org/Policies/API_to_Avoid#QWidget::showFullScreen.2FMaximized.2FMinimized.2FNormal.28.29)
				setWindowState(windowState() | Qt::WindowFullScreen);
				applied = true;
			}
			else
			{
				QStringList splitResolution = resolution.split("x", QString::SkipEmptyParts, Qt::CaseInsensitive);
				
				if(splitResolution.size() == 2)
				{
					bool widthValid = false;
					int width = splitResolution.at(0).toInt(&widthValid);
					widthValid &= (width > 10) && (width < 10000);
					
					bool heightValid = false;
					int height = splitResolution.at(1).toInt(&heightValid);
					heightValid &= (height > 10) && (height < 10000);

					if(widthValid && heightValid)
					{
						setWindowState(windowState() & ~Qt::WindowFullScreen); //Use this as above. This time in addition to resize().
						resize(width, height);
						Application::centerWidget(this);
						applied = true;
					}
				}			
			}
		}

		if(applied)
		{
			qApp->_systemLog()->logMessage("Set main window resolution to \"" + resolution + "\"", LL_INFO);
		}
		else
		{
			qApp->_systemLog()->logMessage("Failed to set main window resolution. The list of resolutions was:", LL_WARNING);
			foreach (QString res, windowModes)
			{
				qApp->_systemLog()->logMessage("    \"" + res + "\"", LL_WARNING);
			}
			qApp->_systemLog()->logMessage("The requested index was: \"" + QString::number(selectedWindowMode) + "\"", LL_WARNING);
		}
		return applied;
	}*/
}
