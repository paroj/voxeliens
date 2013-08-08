#ifndef THERMITE_APPLICATION_H_
#define THERMITE_APPLICATION_H_

#include <OgreLog.h>

#include <QApplication>

#include "Object.h"

class QSettings;

class TankWarsViewWidget;

namespace Thermite
{
	class ViewWidget;
	class FPSDialog;
	class GraphicsSettingsWidget;
	class Log;
	class LogManager;
	class SettingsDialog;
	class AbstractSettingsWidget;

	class Application : public QApplication
	{
		Q_OBJECT

	public:
		/// Creates an instance of the Application class.
		Application(int & argc, char ** argv);
		/// Destroys an instance of the Application class
		~Application();

		///\name Getters
		//@{
		/// The total number of frames rendered
		unsigned int frameCount(void) const;
		/// Get the OGRE RenderWindow for adding viewports   
		//Ogre::RenderWindow* ogreRenderWindow(void) const;
		/// Access the application settings
		QSettings* settings(void) const;
		//@}

		///\name Setters
		//@{
		/// Sets the period between sucessive updates.
		void setAutoUpdateInterval(int intervalInMilliseconds);
		/// Controls whether QtOgre periodically calls update().
		void setAutoUpdateEnabled(bool autoUpdateEnabled);
		//@}

		void addResourceDirectory(const QString& directoryName);
		
		//Static functions
		/// Utility function to center a widget.
		static void centerWidget(QWidget* widgetToCenter, QWidget* parent = 0);

		/// Shows a message box with an 'Info' icon and 'Information' in the title bar.
		static void showInfoMessageBox(const QString& text);
		/// Shows a message box with a 'Warning' icon and 'Warning' in the title bar.
		static void showWarningMessageBox(const QString& text);
		/// Shows a message box with an 'Error' icon and 'Error' in the title bar.
		static void showErrorMessageBox(const QString& text);

	public slots:
		void applySettings(void);
		virtual void shutdown(void);
		virtual void update(void);	

	public:
		QList<ViewWidget*> mViewWidgets;
		QList<Object*> mObjectList;

		//Ogre Stuff
		Ogre::RenderSystem* mActiveRenderSystem;
		Ogre::Root* mRoot;

		//Misc
		unsigned int mFrameCounter;
		QTimer* mAutoUpdateTimer;
		QSettings* mSettings;
		bool mAutoUpdateEnabled;
		bool mIsInitialised;

	private:
		void initialiseRenderSystem(void);
	};
}

//This redefines qApp, causing it to return an Application pointer instead of a QApplication one.
//This is useful, for example, to access the logging system. This is done in the same way that
//Qt does it to get a QApplication rather than a QCoreApplication, so it should be legitimate.
#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Thermite::Application *>(QCoreApplication::instance()))

#endif /*THERMITE_APPLICATION_H_*/
