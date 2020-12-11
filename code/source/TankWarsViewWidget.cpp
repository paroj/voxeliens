/*******************************************************************************
Copyright (c) 2012-2013 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*******************************************************************************/

#include "TankWarsViewWidget.h"

#include "FixedShadowCameraSetup.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MusicPlayer.h"
#include "TaskProcessorThread.h"
#include "SurfaceMeshDecimationTask.h"
#include "SurfaceMeshExtractionTask.h"
#include "SurfacePatchRenderable.h"
#include "PolyVoxCore/Material.h"
#include "QStringIODevice.h"
#include "TankWarsApplication.h"
#include "TextManager.h"
#include "VolumeManager.h"
#include "Utility.h"

#include "Application.h"
#include "Config.h"

#include "PolyVoxCore/RaycastWithCallback.h"

#include "TerrainGeneration.h"

#include "PausedScreen.h"
#include "PlayScreen.h"

#include <QDesktopServices>
#include <QDirIterator>
#include <QKeyEvent>
#include <qglobal.h>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMovie>
#include <QMutex>
#include <QSettings>
#include <QThreadPool>
#include <QTimer>
#include <QUrl>
#include <QWaitCondition>

#include <qmath.h>

#include "Application.h"

#include <QCloseEvent>

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

using namespace std;
using namespace PolyVox;
using namespace Ogre;
using namespace Thermite;


TankWarsViewWidget::TankWarsViewWidget(QWidget* parent, Qt::WindowFlags f)
:OgreWidget(parent, f)
{	
}

TankWarsViewWidget::~TankWarsViewWidget()
{
	qApp->mMusicPlayer->fadeOffSong();

	mVoxeliensSettings->setValue("graphics/isMaximized", isMaximized());

	if(isMaximized())
	{
		//Do this so we get the correct window dimensions.
		showNormal();
	}

	if(IsFullGame == false)
	{
		QMessageBox msgBox(QMessageBox::Question, "Thank you for playing Voxeliens!", "We hope you enjoyed the demo. Would you like to visit our website for the full version of the game?", QMessageBox::Yes | QMessageBox::No, this);
		int ret = msgBox.exec();

		if(ret == QMessageBox::Yes)
		{
			QDesktopServices::openUrl(QUrl(BuyFullGameLink));
		}
	}

	mVoxeliensSettings->setValue("graphics/width", QString::number(width()));
	mVoxeliensSettings->setValue("graphics/height", QString::number(height()));
	mVoxeliensSettings->setValue("graphics/xpos", QString::number(frameGeometry().left()));
	mVoxeliensSettings->setValue("graphics/ypos", QString::number(frameGeometry().top()));
	
	mVoxeliensSettings->sync();
	delete mVoxeliensSettings;
	
	delete mMainMenuScreen;
	delete mHighScoreScreen;
	delete mPlayScreen;
	delete mPausedScreen;
	delete mVolume;
}

void TankWarsViewWidget::initialise(void)
{
	ViewWidget::initialise();

	//Load the settings file. If it doesn't exist it is created.
	mVoxeliensSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "VolumesOfFun", "Voxeliens");

	mSettingsDialog = new ::SettingsDialog(this);
	
	//Our main volume
	//Object* volumeObject = new Object();
	mVolume = new Thermite::Volume(128, 32, 128);

	//Mouse handling
	setMouseTracking(true);
	//setCursor( QCursor( Qt::BlankCursor ) );

	mOgreSceneManager->setShadowCameraSetup(ShadowCameraSetupPtr(new FixedShadowCameraSetup()));

	mPausedScreen = new PausedScreen(this);
	mPlayScreen = new PlayScreen(this);
	mHighScoreScreen = new HighScoreScreen(this);
	mMainMenuScreen = new MainMenuScreen(this);
	mCurrentScreen = mMainMenuScreen;

	mCurrentScreen->initialise();
}

void TankWarsViewWidget::closeEvent(QCloseEvent* event)
{	
	qDebug() << "======================================== Closing Voxeliens ========================================";
	event->accept();
}