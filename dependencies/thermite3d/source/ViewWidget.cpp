#include "ViewWidget.h"

#include "OgreWidget.h"

#include "SkyBox.h"
#include "TaskProcessorThread.h"
#include "SurfaceMeshDecimationTask.h"
#include "SurfaceMeshExtractionTask.h"
#include "SurfacePatchRenderable.h"
#include "PolyVoxCore/Material.h"
#include "QStringIODevice.h"
#include "TextManager.h"
#include "VolumeManager.h"
#include "Utility.h"

#include "Application.h"
#include "FPSDialog.h"
#include "LogManager.h"

#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>

#include <QDirIterator>
#include <QKeyEvent>
#include <qglobal.h>
#include <QMouseEvent>
#include <QMovie>
#include <QMutex>
#include <QSettings>
#include <QThreadPool>
#include <QTimer>
#include <QWaitCondition>

#include <qmath.h>

#include "Application.h"

#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

#include <QCloseEvent>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

using namespace std;
using namespace PolyVox;

namespace Thermite
{
	ViewWidget::ViewWidget(QWidget* parent, Qt::WindowFlags f)
	:QWidget(parent, f)

		//Scene representation
		,mCamera(0)
		,mVolume(0)
		//,mSkyBox(0)
		//,mVolLastUploadedTimeStamps(0)
		//,mObjectList(0)
		

		//Ogre's scene representation
		
		//,m_volOgreSceneNodes(0)
		,mCameraSceneNode(0)
		,mOgreCamera(0)
		,mMainViewport(0)
		,mOgreSceneManager(0)
		,m_axisNode(0)

		//User interface
		,m_pThermiteLogoMovie(0)
		,m_pThermiteLogoLabel(0)

		//Other
		,mFirstFind(false)
	{	
		mCamera = new Camera(0);			
		//mSkyBox = new SkyBox(0);
		FPSDialog* fpsDialog = new FPSDialog(this);
		//fpsDialog->show();

		//Accept input focus
		setFocusPolicy(Qt::StrongFocus);
	}

	ViewWidget::~ViewWidget()
	{
	}

	void ViewWidget::initialise(void)
	{
		//Set the main window icon
		QIcon mainWindowIcon(QPixmap(QString::fromUtf8(":/images/thermite_logo.svg")));
		setWindowIcon(mainWindowIcon);

		//We have to create a scene manager and viewport here so that the screen
		//can be cleared to black befre the Thermite logo animation is played.
		mOgreSceneManager = Ogre::Root::getSingletonPtr()->createSceneManager(Ogre::ST_GENERIC, "OgreSceneManager");

		mOgreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		mOgreSceneManager->setShadowFarDistance(200.0f);		
		mOgreSceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
		mOgreSceneManager->setShadowTextureCount(1);
		mOgreSceneManager->setShadowTextureSize(512);
		mOgreSceneManager->setShadowCasterRenderBackFaces(true);
		//mOgreSceneManager->setShowDebugShadows(true);


		mOgreCamera = mOgreSceneManager->createCamera("OgreCamera");
		mOgreCamera->setFOVy(Ogre::Radian(1.0));
		mOgreCamera->setNearClipDistance(1.0);
		mOgreCamera->setFarClipDistance(5000);

		mCameraSceneNode = mOgreSceneManager->createSceneNode("Camera Scene Node");
		//mCameraSceneNode->attachObject(mOgreCamera);

		OgreWidget* pOgreWidget = dynamic_cast<OgreWidget*>(this);

		mMainViewport = pOgreWidget->getOgreRenderWindow()->addViewport(mOgreCamera);
		mMainViewport->setBackgroundColour(Ogre::ColourValue::Black);

		createAxis();
		//Hide the node
		m_axisNode->setVisible(false);

		//Set up and start the thermite logo animation. This plays while we initialise.
		//playStartupMovie();

		//Load engine built-in resources
		//addResourceDirectory("./resources/", "Thermite");
		//Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Thermite");
		//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		//Some Ogre related stuff we need to set up
		Ogre::Root::getSingletonPtr()->addMovableObjectFactory(new SurfacePatchRenderableFactory);
		VolumeManager* vm = new VolumeManager;
		vm->m_pProgressListener = new VolumeSerializationProgressListenerImpl();

		TextManager* sm = new TextManager;	
		
		//loadApp(QString::fromAscii("TankWars"));

		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		//Note: Shadow caster material is actually broken for the
		//fireballs, but we don't really want them to cast shadows anyway.
		mOgreSceneManager->setShadowTextureCasterMaterial(Ogre::MaterialManager::getSingleton().getByName("ShadowCasterMaterial"));

		/*mLogManager = new LogManager(this);
		mMainLog = mLogManager->createLog("Main");
		mLogManager->show();
		mMainLog->logMessage("Hello", LL_WARNING);*/
	}

	/*void ViewWidget::log(QString string)
	{
		mMainLog->logMessage(string, LL_WARNING);
	}*/

	void ViewWidget::update(void)
	{
		if(mVolume)
		{
			mVolume->updatePolyVoxGeometry(QVector3D(mOgreCamera->getPosition().x, mOgreCamera->getPosition().y, mOgreCamera->getPosition().z));
		}

		if(mOgreSceneManager)
		{			
			QListIterator<Object*> objectIter(qApp->mObjectList);
			while(objectIter.hasNext())
			{				
				Object* pObj = objectIter.next();
				if(pObj->mComponent)
				{
					//Use the objects address to build unique names
					std::string objAddressAsString = QString::number(reinterpret_cast<qulonglong>(pObj), 16).toStdString();

					RenderComponent* renderComponent = dynamic_cast<RenderComponent*>(pObj->mComponent);
					if(renderComponent)
					{
						renderComponent->update();
					}
				}
			}

			if(mVolume)
			{
				mVolume->update();
				m_axisNode->setScale(mVolume->m_pPolyVoxVolume->getWidth()+2, mVolume->m_pPolyVoxVolume->getHeight()+2, mVolume->m_pPolyVoxVolume->getDepth()+2);
			}

			

			//Update the camera
			if(mCamera && mOgreCamera)
			{
				if(mCameraSceneNode->numAttachedObjects() == 0)
				{
					mCameraSceneNode->attachObject(mOgreCamera);
				}

				QMatrix4x4 qtTransform = mCamera->transform();
				Ogre::Matrix4 ogreTransform;
				for(int row = 0; row < 4; ++row)
				{
					Ogre::Real* rowPtr = ogreTransform[row];
					for(int col = 0; col < 4; ++col)
					{
						Ogre::Real* colPtr = rowPtr + col;
						*colPtr = qtTransform(row, col);
					}
				}

				mCameraSceneNode->setOrientation(ogreTransform.extractQuaternion());
				mCameraSceneNode->setPosition(ogreTransform.getTrans());

				mOgreCamera->setFOVy(Ogre::Radian(mCamera->fieldOfView()));
			}
		}

		mCurrentScreen->preUpdate();
		mCurrentScreen->update();
		mCurrentScreen->postUpdate();

		QWidget::update();
	}

	void ViewWidget::shutdown(void)
	{
		Ogre::Root::getSingleton().destroySceneManager(mOgreSceneManager);
	}

	void ViewWidget::createAxis(void)
	{
		//Create the main node for the axes
		m_axisNode = mOgreSceneManager->getRootSceneNode()->createChildSceneNode();

		//Create remainder of box		
		Ogre::ManualObject* axis = mOgreSceneManager->createManualObject("Axis");
		axis->begin("BaseWhiteNoLighting",Ogre::RenderOperation::OT_LINE_LIST);
		axis->position(0.0,	0.0, 0.0);	axis->colour(0.0, 0.0, 1.0);	axis->position(0.0,	0.0, 1.0);	axis->colour(0.0, 0.0, 1.0);
		axis->position(0.0,	1.0, 0.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(0.0,	1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(1.0,	0.0, 0.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	0.0, 1.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(1.0,	1.0, 0.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0, 1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);

		axis->position(0.0, 0.0, 0.0);	axis->colour(0.0, 1.0, 0.0);	axis->position(0.0,	1.0, 0.0);	axis->colour(0.0, 1.0, 0.0);
		axis->position(0.0,	0.0, 1.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(0.0,	1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(1.0,	0.0, 0.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	1.0, 0.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(1.0,	0.0, 1.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);

		axis->position(0.0,	0.0, 0.0);	axis->colour(1.0, 0.0, 0.0);	axis->position(1.0,	0.0, 0.0);	axis->colour(1.0, 0.0, 0.0);
		axis->position(0.0,	0.0, 1.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	0.0, 1.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(0.0,	1.0, 0.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	1.0, 0.0);	axis->colour(1.0, 1.0, 1.0);
		axis->position(0.0,	1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);	axis->position(1.0,	1.0, 1.0);	axis->colour(1.0, 1.0, 1.0);
		axis->end();

		//Attach the box to the node
		Ogre::SceneNode *axisNode = m_axisNode->createChildSceneNode();
		axisNode->attachObject(axis);	
	}

	void ViewWidget::playStartupMovie(void)
	{
		m_pThermiteLogoMovie = new QMovie(QString::fromUtf8(":/animations/thermite_logo.mng"));
		m_pThermiteLogoLabel = new QLabel(this, Qt::FramelessWindowHint | Qt::Tool);
		connect(m_pThermiteLogoMovie, SIGNAL(finished(void)), this, SLOT(showLastMovieFrame(void)));
		m_pThermiteLogoLabel->setMovie(m_pThermiteLogoMovie);
		m_pThermiteLogoMovie->jumpToFrame(0);
		m_pThermiteLogoLabel->resize(m_pThermiteLogoMovie->currentImage().size());
		m_pThermiteLogoLabel->show();
		m_pThermiteLogoMovie->start();
	}

	void ViewWidget::showLastMovieFrame(void)
	{
		QTimer::singleShot(1000, this, SLOT(deleteMovie()));
	}

	void ViewWidget::deleteMovie(void)
	{
		if(m_pThermiteLogoLabel != 0)
		{
			delete m_pThermiteLogoLabel;
			m_pThermiteLogoLabel = 0;
		}
		if(m_pThermiteLogoMovie != 0)
		{
			delete m_pThermiteLogoMovie;
			m_pThermiteLogoMovie = 0;
		}
	}

	QVector3D ViewWidget::getPickingRayOrigin(int x, int y)
	{
		float actualWidth = mOgreCamera->getViewport()->getActualWidth();
		float actualHeight = mOgreCamera->getViewport()->getActualHeight();

		float fNormalisedX = x / actualWidth;
		float fNormalisedY = y / actualHeight;

		Ogre::Ray pickingRay = mOgreCamera->getCameraToViewportRay(fNormalisedX, fNormalisedY);

		return QVector3D(pickingRay.getOrigin().x, pickingRay.getOrigin().y, pickingRay.getOrigin().z);
	}

	QVector3D ViewWidget::getPickingRayDir(int x, int y)
	{
		float actualWidth = mOgreCamera->getViewport()->getActualWidth();
		float actualHeight = mOgreCamera->getViewport()->getActualHeight();

		float fNormalisedX = x / actualWidth;
		float fNormalisedY = y / actualHeight;

		Ogre::Ray pickingRay = mOgreCamera->getCameraToViewportRay(fNormalisedX, fNormalisedY);

		return QVector3D(pickingRay.getDirection().x, pickingRay.getDirection().y, pickingRay.getDirection().z);
	}

	void ViewWidget::keyPressEvent(QKeyEvent* event)
	{
		mCurrentScreen->keyPressEvent(event);
	}

	void ViewWidget::keyReleaseEvent(QKeyEvent* event)
	{
		mCurrentScreen->keyReleaseEvent(event);
	}

	void ViewWidget::mousePressEvent(QMouseEvent* event)
	{
		mCurrentScreen->mousePressEvent(event);
	}

	void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
	{
		mCurrentScreen->mouseReleaseEvent(event);
	}

	void ViewWidget::mouseMoveEvent(QMouseEvent* event)
	{
		mCurrentScreen->mouseMoveEvent(event);
	}

	void ViewWidget::wheelEvent(QWheelEvent* event)
	{
		mCurrentScreen->wheelEvent(event);
	}

	Screen* ViewWidget::setScreen(Screen* screen)
	{
		Screen* previousScreen = mCurrentScreen;
		mCurrentScreen = screen;

		previousScreen->shutdown();
		mCurrentScreen->initialise();
		return previousScreen;
	}
}
