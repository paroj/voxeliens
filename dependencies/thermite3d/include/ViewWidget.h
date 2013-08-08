#ifndef THERMITE_VIEWWIDGET_H_
#define THERMITE_VIEWWIDGET_H_

#include "PolyVoxUtil/Serialization.h"

#include "Camera.h"
#include "Entity.h"
#include "Globals.h"
#include "Light.h"

#include "scriptable/Volume.h"
#include "ThermiteForwardDeclarations.h"

#include "Screen.h"

#include "QtOgreForwardDeclarations.h"

#include <OgrePrerequisites.h>
#include <OgreTexture.h>

#include <QHash>

#include <QTime>

#include <QMovie>
#include <QLabel>
#include <QObject>

#include <list>
#include <queue>

class QWidget;

namespace Thermite
{

	class ViewWidget : public QWidget
	{
		Q_OBJECT

	public:
		ViewWidget(QWidget* parent=0, Qt::WindowFlags f=0);
		~ViewWidget();

		virtual void initialise(void);
		virtual void update(void);
		virtual void shutdown(void);

		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);

		void mousePressEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

		void wheelEvent(QWheelEvent* event);

		Screen* setScreen(Screen* screen);

	public:
		

		void createAxis(void);

	public slots:

		//Don't like having these functions here - really they should be inside camera or something. But they are
		//using Ogre methods which aren't available in the scriptable classes. Eventually they should be rewritten
		//without the utility classes. Also, there are two seperate functions because I couldn't pass QVector3D
		//by reference. Need to look at registering this type...
		QVector3D getPickingRayOrigin(int x, int y);
		QVector3D getPickingRayDir(int x, int y);

	private slots:

		void playStartupMovie(void);
		void showLastMovieFrame(void);
		void deleteMovie(void);

	public:

		/*LogManager* mLogManager;
		Log* mMainLog;
		void log(QString string);*/

		//Scene representation
		Camera* mCamera;
		//SkyBox* mSkyBox;
		

		Volume* mVolume;

		//Ogre's scene representation
		
		
		Ogre::SceneNode* mCameraSceneNode;
		Ogre::Camera* mOgreCamera;
		Ogre::Viewport* mMainViewport;
		Ogre::SceneManager* mOgreSceneManager;
		Ogre::SceneNode* m_axisNode;

		//User interface
		QMovie* m_pThermiteLogoMovie;
		QLabel* m_pThermiteLogoLabel;

		//Other
		bool mFirstFind;

		//Screen
		Screen* mCurrentScreen;
	};
}

#endif /*THERMITE_VIEWWIDGET_H_*/
