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

#include "MainMenuScreen.h"

#include "Config.h"

#include <QDesktopServices>
#include <QMouseEvent>
#include <qmath.h>
#include <QUrl>

#include "Mouse.h"
#include "SettingsDialog.h"
#include "TankWarsApplication.h"
#include "TankWarsViewWidget.h"

MainMenuScreen::MainMenuScreen(TankWarsViewWidget* tankWarsViewWidget)
	:GameScreen(tankWarsViewWidget)
{	
	mFirstShowing = true;
}

MainMenuScreen::~MainMenuScreen()
{
	Mix_HaltChannel(-1);
	Mix_FreeChunk(mThudSound);
	Mix_FreeChunk(mClickSound);
}

void MainMenuScreen::initialise()
{
	qDebug() << "MainMenuScreen::initialise()";
	GameScreen::initialise();

	Ogre::Vector3 baseMinExtents = Ogre::Vector3(0, -5, 63);
	Ogre::Vector3 baseMaxExtents = Ogre::Vector3(128, 5, 65);

	mNewGameText = new Text3D();
	mNewGameText->setPosition(QVector3D(36, 50, 64)); //(indent, height, dist from camera)
	mNewGameTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,50,0), baseMaxExtents + Ogre::Vector3(0,50,0));
	mNewGameText->setText("New Game");

	mSettingsText = new Text3D();
	mSettingsText->setPosition(QVector3D(36, 40, 64)); //(indent, height, dist from camera)
	mSettingsTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,40,0), baseMaxExtents + Ogre::Vector3(0,40,0));
	mSettingsText->setText("Settings");

	if(IsFullGame)
	{
		mBuyFullGameText = new Text3D();
		mBuyFullGameText->setPosition(QVector3D(18, 30, 64)); //(indent, height, dist from camera)
		mBuyFullGameTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,30,0), baseMaxExtents + Ogre::Vector3(0,30,0));
		mBuyFullGameText->setText("XXX");
		mBuyFullGameText->setVisible(false);

		mQuitGameText = new Text3D();
		mQuitGameText->setPosition(QVector3D(50, 30, 64)); //(indent, height, dist from camera)
		mQuitGameTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,30,0), baseMaxExtents + Ogre::Vector3(0,30,0));
		mQuitGameText->setText("Quit");
	}
	else
	{
		mBuyFullGameText = new Text3D();
		mBuyFullGameText->setPosition(QVector3D(18, 30, 64)); //(indent, height, dist from camera)
		mBuyFullGameTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,30,0), baseMaxExtents + Ogre::Vector3(0,30,0));
		mBuyFullGameText->setText("Buy Full Game");

		mQuitGameText = new Text3D();
		mQuitGameText->setPosition(QVector3D(50, 20, 64)); //(indent, height, dist from camera)
		mQuitGameTextAABB.setExtents(baseMinExtents + Ogre::Vector3(0,20,0), baseMaxExtents + Ogre::Vector3(0,20,0));
		mQuitGameText->setText("Quit");
	}

	mCurrentText = 0;
	mCurrentTextVisible = true;

	mFlashingTimer = new QTimer(this);
	QObject::connect(mFlashingTimer, SIGNAL(timeout()), this, SLOT(toggleVisibility()));
	mFlashingTimer->start(250);

	if(mFirstShowing)
	{
		//No delay as there as no previous screen.
		QTimer::singleShot(0, this, SLOT(startPlayingSong(void)));
		mFirstShowing = false;
	}
	else
	{
		//Delay while high score music fades.
		QTimer::singleShot(2000, this, SLOT(startPlayingSong(void)));
	}

	cameraFocusPoint = QVector3D(64, 16, 64);

	mThudSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/Thud.wav").toLatin1().data());
	mClickSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/Click.wav").toLatin1().data());

	//generateMap(TerrainTypes::Desert, 87987, mVolume); //Used for menu and first level of the game
	mTankWarsViewWidget->mVolume->readFromFile(qApp->mResourcePath+"volumes/level0.vol"); //Used for menu and first level of the game
}

void MainMenuScreen::update()
{
	GameScreen::update();

	float rotateAmount = 10.0f;
	float rotateSpeed = 0.263f;

	float elevateAmount = 5.0f;
	float elevateSpeed = 0.157f;
	float elevateInitial = 15.0f;

	cameraDistance = 145.0; //Reset camera distance

	cameraRotationAngle = qSin(currentTimeInSeconds * rotateSpeed) * rotateAmount;
	cameraElevationAngle = elevateInitial + (qSin(currentTimeInSeconds * elevateSpeed) * elevateAmount);

	cameraNode->setOrientation(QQuaternion());	
	cameraNode->yaw(-cameraRotationAngle);
	cameraNode->pitch(-cameraElevationAngle);

	cameraNode->setPosition(cameraFocusPoint);

	mTankWarsViewWidget->mCamera->setOrientation(QQuaternion());
	mTankWarsViewWidget->mCamera->setPosition(QVector3D(0,0,cameraDistance));
}

void MainMenuScreen::shutdown()
{
	qDebug() << "MainMenuScreen::shutdown()";
	GameScreen::shutdown();

	mFlashingTimer->stop();

	delete mNewGameText;
	delete mSettingsText;
	delete mBuyFullGameText;
	delete mQuitGameText;
}

void MainMenuScreen::moveUp(void)
{
	if(mCurrentText == mNewGameText)
		mCurrentText = mQuitGameText;
	else if(mCurrentText == mQuitGameText)
	{
		if(IsFullGame)
		{
			mCurrentText = mSettingsText;
		}
		else
		{
			mCurrentText = mBuyFullGameText;
		}
	}
	else if(mCurrentText == mBuyFullGameText)
		mCurrentText = mSettingsText;
	else if(mCurrentText == mSettingsText)
		mCurrentText = mNewGameText;
	else if(mCurrentText == 0)
		mCurrentText = mQuitGameText;

	currentTextUpdated();
}

void MainMenuScreen::moveDown(void)
{
	if(mCurrentText == mNewGameText)
		mCurrentText = mSettingsText;
	else if(mCurrentText == mSettingsText)
	{
		if(IsFullGame)
		{
			mCurrentText = mQuitGameText;
		}
		else
		{
			mCurrentText = mBuyFullGameText;
		}
	}
	else if(mCurrentText == mBuyFullGameText)
		mCurrentText = mQuitGameText;
	else if(mCurrentText == mQuitGameText)
		mCurrentText = mNewGameText;
	else if(mCurrentText == 0)
		mCurrentText = mNewGameText;

	currentTextUpdated();
}

void MainMenuScreen::keyPressEvent(QKeyEvent* event)
{
	GameScreen::keyPressEvent(event);

	if(event->isAutoRepeat())
	{
		return;
	}

	if(event->key() == Qt::Key_Escape)
	{
		mTankWarsViewWidget->close();
	}

	if((event->key() == Qt::Key_Up) || (event->nativeVirtualKey() == mTankWarsViewWidget->mVoxeliensSettings->value("input/forwardKey", defaultForwardVirtualKey()).toUInt()))
	{
		moveUp();
	}

	if((event->key() == Qt::Key_Down) || (event->key() == mTankWarsViewWidget->mVoxeliensSettings->value("input/backKey", defaultBackVirtualKey()).toUInt()))
	{
		moveDown();
	}

	if((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
	{
		currentTextSelected();
	}
}

void MainMenuScreen::wheelEvent(QWheelEvent* event)
{
	if(event->delta() > 0)
	{
		moveUp();
	}
	if(event->delta() < 0)
	{
		moveDown();
	}
}

void MainMenuScreen::mouseMoveEvent(QMouseEvent* event)
{
	float xScreen = event->posF().x() / mTankWarsViewWidget->width();
	float yScreen = event->posF().y() / mTankWarsViewWidget->height();
	//yScreen = 1.0f - yScreen;

	Ogre::Ray ray = mTankWarsViewWidget->mOgreCamera->getCameraToViewportRay(xScreen, yScreen);

	ray.setDirection(ray.getDirection() * 1000.0f);

	Text3D* oldText = mCurrentText;

	if(ray.intersects(mNewGameTextAABB).first)
	{
		mCurrentText = mNewGameText;
	}
	else if(ray.intersects(mSettingsTextAABB).first)
	{
		mCurrentText = mSettingsText;
	}
	else if((ray.intersects(mBuyFullGameTextAABB).first) && (IsFullGame == false))
	{
		mCurrentText = mBuyFullGameText;
	}
	else if(ray.intersects(mQuitGameTextAABB).first)
	{
		mCurrentText = mQuitGameText;
	}
	else
	{
		mCurrentText = 0;
	}

	if(oldText != mCurrentText)
	{
		currentTextUpdated();
	}
}

void MainMenuScreen::mousePressEvent(QMouseEvent* event)
{
	GameScreen::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
	{
		currentTextSelected();
	}
}

void MainMenuScreen::currentTextUpdated(void)
{
	mNewGameText->setVisible(true);
	mSettingsText->setVisible(true);
	if(IsFullGame == false)
	{
		mBuyFullGameText->setVisible(true);
	}
	mQuitGameText->setVisible(true);

	if(mCurrentText)
	{
		mCurrentText->setVisible(false);
		mCurrentTextVisible = false;

		Mix_PlayChannel(-1, mThudSound, 0);
	}
}

void MainMenuScreen::currentTextSelected(void)
{
	Mix_PlayChannel(-1, mClickSound, 0);

	if(mCurrentText == mNewGameText)
	{
		mTankWarsViewWidget->setScreen(mTankWarsViewWidget->mPlayScreen);
	}
	else if(mCurrentText == mSettingsText)
	{
		bool oldFullscreenMode = mTankWarsViewWidget->mIsFullscreenMode;
		qDebug() << "Ensuring windowed mode to show settings dialog...";
		mTankWarsViewWidget->changeWindowSetup(false);
		mTankWarsViewWidget->mSettingsDialog->readFromSettings(mTankWarsViewWidget->mVoxeliensSettings);
		if(mTankWarsViewWidget->mSettingsDialog->exec() == QDialog::Accepted)
		{
			mTankWarsViewWidget->mSettingsDialog->writeToSettings(mTankWarsViewWidget->mVoxeliensSettings);
			mTankWarsViewWidget->mVoxeliensSettings->sync();

			bool runFullscreen = mTankWarsViewWidget->mVoxeliensSettings->value("graphics/runFullscreen", false).toBool();
			if(runFullscreen)
			{
				qDebug() << "Switching to fullscreen mode";
				mTankWarsViewWidget->changeWindowSetup(true);
			}
			else
			{
				qDebug() << "Switching to windowed mode";
				mTankWarsViewWidget->changeWindowSetup(false);
			}
		}
		else
		{
			qDebug() << "Restoring windowed mode to state before showing settings dialog";
			mTankWarsViewWidget->changeWindowSetup(oldFullscreenMode);
		}
	}
	else if(mCurrentText == mBuyFullGameText)
	{
		qDebug() << "Ensuring windowed mode to show website";
		mTankWarsViewWidget->mVoxeliensSettings->setValue("graphics/runFullscreen", false);
		mTankWarsViewWidget->changeWindowSetup(false);
		QDesktopServices::openUrl(QUrl(BuyFullGameLink));
	}
	else if(mCurrentText == mQuitGameText)
	{
		mTankWarsViewWidget->close();
	}
}

void MainMenuScreen::toggleVisibility(void)
{
	if(mCurrentText)
	{
		mCurrentText->setVisible(mCurrentTextVisible);
		mCurrentTextVisible = !mCurrentTextVisible;
	}
}

void MainMenuScreen::startPlayingSong(void)
{
	if(mTankWarsViewWidget->mCurrentScreen == this) //Avoid problems when quickly jumping between screens.
	{
		qApp->mMusicPlayer->playSong("Mainmenu.ogg");
	}
}
