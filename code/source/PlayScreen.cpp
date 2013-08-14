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

#include "PlayScreen.h"
#include "Config.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RaycastWithCallback.h"

#include "Globals.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Object.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QTextStream>

#include <qmath.h>

#include "TankWarsApplication.h"
#include "TankWarsViewWidget.h"

#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "OgreRenderWindow.h"
#include "OgreSceneManager.h"

using namespace PolyVox;

const float delayBetweenShots = 0.5f;
const float delayBetweenShotsRapidFire = 0.1f;
const int powerUpWearOffTimeInMs = 8000;
const int powerUpWaitingTimeInMs = 10000;
const int powerUpSpawnFrequency = 10; //Big numbers are less frequent
const int initialSpareLives = 3;
const float missileSpeed = 200.0f;
const float sprayMissileHorzSpeed = 15.0f;
const int TankYPos = 4;
const int WorldSizeX = 128;
const int WorldSizeZ = 128;
const float RailgunHiddenHeight = -40;
const float RailgunVisibleHeight = 50;
const float TankSpeed = 50.0f;
const int PointsPerSpareSecond = 10;
const int PointsLostPerCrash = 500;
const int PressAnyKeyDelayInMS = 3000;
const int PointsPerExtraLife = 10000;

PlayScreen::PlayScreen(TankWarsViewWidget* tankWarsViewWidget)
	:GameScreen(tankWarsViewWidget)
	,mBlankCursor(0)
	,mCursorHidden(false)
	,mPaused(false)
	,mBombOffset(0)
	,mCurrentPowerUp(PowerUpTypes::Last)
	,mDifficultyMultiplier(1.0)
{	
	connect(&mFlashTankTimer, SIGNAL(timeout()), this, SLOT(toggleTankVisible()));
	connect(&mClearActivePowerUpTimer, SIGNAL(timeout()), this, SLOT(clearActivePowerUp()));
	connect(&mClearWaitingPowerUpTimer, SIGNAL(timeout()), this, SLOT(hideWaitingPowerUps()));
	connect(&mShowPressAnyKeyTimer, SIGNAL(timeout()), this, SLOT(showPressAnyKey()));


	for(int z = 0; z < EnemyArrayDepth; z++)
	{
		for(int y = 0; y < EnemyArrayHeight; y++)
		{
			for(int x = 0; x < EnemyArrayWidth; x++)
			{
				mEnemies[x][y][z] = 0;
			}
		}
	}

	for(int ct = 0; ct < MaxNoOfBombs; ct++)
	{
		Thermite::Object* bombObject = new Thermite::Object();
		Thermite::Entity* bombEntity = new Thermite::Entity("shell.mesh", "MeshMaterial", bombObject);
		bombEntity->setCastsShadows(false);
		bombObject->lookAt(bombObject->position() - QVector3D(0,16,0));
		qApp->mObjectList.append(bombObject);
		mBombList.append(bombObject);
	}

	//Tank
	mTankObject = new Object();
	mTankEntity = new Thermite::Entity("tank.mesh", "MeshMaterial", mTankObject);
	qApp->mObjectList.append(mTankObject);

	//Railgun
	mRailgunObject = new Object();
	Thermite::Entity* railgunEntity = new Thermite::Entity("PurpleRailgun.mesh", "MeshMaterial", mRailgunObject);
	mRailgunObject->setSize(QVector3D(0.95, 1.0, 0.95)); //Stop z-fighting with tank
	mRailgunObject->setPosition(QVector3D(0, RailgunVisibleHeight, 0)); //x and z will get set later.
	mRailgunObject->mComponent->setEnabled(false);
	qApp->mObjectList.append(mRailgunObject);

	//Ready to shoot markers
	mReadyToShootNormalObject = new Object();
	Thermite::Entity* readyToShootNormalEntity = new Thermite::Entity("WhiteCube.mesh", "MeshMaterial", mReadyToShootNormalObject);
	qApp->mObjectList.append(mReadyToShootNormalObject);

	mReadyToShootRapidFireObject = new Object();
	Thermite::Entity* readyToShootRapidFireEntity = new Thermite::Entity("OrangeCube.mesh", "MeshMaterial", mReadyToShootRapidFireObject);
	qApp->mObjectList.append(mReadyToShootRapidFireObject);

	mReadyToShootRailgunObject = new Object();
	Thermite::Entity* readyToShootRailgunEntity = new Thermite::Entity("PurpleCube.mesh", "MeshMaterial", mReadyToShootRailgunObject);
	qApp->mObjectList.append(mReadyToShootRailgunObject);

	mReadyToShootSprayObject = new Object();
	Thermite::Entity* readyToShootSprayEntity = new Thermite::Entity("CyanCube.mesh", "MeshMaterial", mReadyToShootSprayObject);
	qApp->mObjectList.append(mReadyToShootSprayObject);

	//Score text
	mScoreText = new Text3D();
	mScoreText->setPosition(QVector3D(0,-10, 127));

	//Life entity
	mLifeObject = new Object();
	mLifeEntity = new Thermite::Entity("tank.mesh", "MeshMaterial", mLifeObject);
	mLifeObject->setPosition(QVector3D(98,-8, 128));
	qApp->mObjectList.append(mLifeObject);

	//Spare lives counter
	mSpareLivesText = new Text3D();
	mSpareLivesText->setPosition(QVector3D(107,-10, 127));

	//Audio
	mMissileSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/missile.wav").toLatin1().data());
	mBombSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/bomb.wav").toLatin1().data());
	mRailgunSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/railgun.wav").toLatin1().data());
	mPowerupSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/powerup.wav").toLatin1().data());
	mPowerdownSound = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/powerdown.wav").toLatin1().data());
	//mExplosionSound = Mix_LoadWAV((audioDirectoryLocation+"audio/effects/explosion.wav").toLatin1().data());
	mNextExplosionSound = 0;
	for(int ct = 0; ct < NoOfExplosionSounds; ct++)
	{
		mExplosionSounds[ct] = Mix_LoadWAV((qApp->mResourcePath+"audio/effects/explosion.wav").toLatin1().data());
	}

	//Level complete text
	mLevelCompleteText = new Text3D();
	mLevelCompleteText->setPosition(QVector3D(5, 60, 64));
	mLevelCompleteText->setText("INITIAL TEXT");
	mLevelCompleteText->setVisible(false);

	mTimeBonusText = new Text3D();
	mTimeBonusText->setPosition(QVector3D(12, 50, 64));
	mTimeBonusText->setText("INITIAL TEXT");
	mTimeBonusText->setVisible(false);

	mDemoCompleteText = new Text3D();
	mDemoCompleteText->setPosition(QVector3D(26, 50, 64));
	mDemoCompleteText->setText("END OF DEMO");
	mDemoCompleteText->setVisible(false);

	mGameOverText = new Text3D();
	mGameOverText->setPosition(QVector3D(33, 50, 64));
	mGameOverText->setText("GAME OVER");
	mGameOverText->setVisible(false);

	mPressAnyKeyText = new Text3D();
	mPressAnyKeyText->setPosition(QVector3D(19, 40, 64));
	mPressAnyKeyText->setText("PRESS ANY KEY");
	mPressAnyKeyText->setVisible(false);

	mEnemyRoot = new Thermite::Object();

	mBlankCursor = new QCursor(Qt::BlankCursor);

	// Create a manual object for 'quit to main menu' dialog
	Ogre::ManualObject* manual = mTankWarsViewWidget->mOgreSceneManager->createManualObject("manual");
 
	// Use identity view/projection matrices
	manual->setUseIdentityProjection(true);
	manual->setUseIdentityView(true);
 
	manual->begin("DialogMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	float dialogAspect = 493.0f / 132.0f; //Based on texture size
	float dialogScaleFactor = 0.1;
	float dialogXPos = dialogScaleFactor * dialogAspect;
	float dialogYPos = dialogScaleFactor;
 
	manual->position(-dialogXPos, -dialogYPos, 0.0);
	manual->textureCoord(0.0, 1.0);
	manual->position( dialogXPos, -dialogYPos, 0.0);
	manual->textureCoord(1.0, 1.0);
	manual->position( dialogXPos,  dialogYPos, 0.0);
	manual->textureCoord(1.0, 0.0);
	manual->position(-dialogXPos,  dialogYPos, 0.0);
	manual->textureCoord(0.0, 0.0);
 
	manual->index(0);
	manual->index(1);
	manual->index(2);
	manual->index(0);
	manual->index(2);
	manual->index(3);
 
	manual->end();
 
	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	manual->setBoundingBox(aabInf);
 
	// Render just before overlays
	manual->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
 
	// Attach to scene
	mQuitToMainMenuDialog = mTankWarsViewWidget->mOgreSceneManager->getRootSceneNode()->createChildSceneNode();
	mQuitToMainMenuDialog->attachObject(manual);
	mQuitToMainMenuDialog->setVisible(false);

	//Start off with no level in play, until we call initialiseLevel().
	mLevelCompleted = true;

	hideAllObjects();
}

PlayScreen::~PlayScreen()
{
	mTankWarsViewWidget->unsetCursor();
	mCursorHidden = false;

	delete mBlankCursor;
	mBlankCursor = 0;
}

void PlayScreen::initialise()
{
	qDebug() << "PlayScreen::initialise()";
	GameScreen::initialise();

	//Initialise level settings
	initialiseLevelSettings();

	//Init score
	mScore = 0;	
	updateScoreText();
	mScoreText->setVisible(true);

	//Init spare lives
	mSpareLives = initialSpareLives;
	updateSpareLivesText();
	mSpareLivesText->setVisible(true);
	mLifeObject->mComponent->setEnabled(true);

	mColCheckLoopX = 0;
	mColCheckLoopZ = 0;

	mPlayerState = PlayerStates::Alive;
	mFlashTankTimer.setInterval(200);	
	mFlashTankTimer.start();

	mCurrentLevel = 0;

	mGameOver = false;

	mTankObject->mComponent->setEnabled(true);

	//Mouse
	mTankWarsViewWidget->setCursor(*mBlankCursor);
	QRect geometry = mTankWarsViewWidget->geometry();
	QCursor::setPos((geometry.bottomRight() + geometry.topLeft()) * 0.5f);
	mouse->setPreviousPosition(mouse->position());

	cameraFocusPoint = QVector3D(64, 16, 64);

	mUpdateCounter = 0;

	mNextExtraLifeTarget = PointsPerExtraLife;

	int difficulty = mTankWarsViewWidget->mVoxeliensSettings->value("difficulty", Difficulties::Normal).toUInt();
	mDifficultyMultiplier = 1.0; //Set default here, in case difficulty is invalid enum due to user trying to tweak setting files.
	switch(difficulty)
	{
	case Difficulties::Easy:
		{
			mDifficultyMultiplier = 0.7;
			break;
		}
	case Difficulties::Normal:
		{
			mDifficultyMultiplier = 1.0;
			break;
		}
	case Difficulties::Hard:
		{
			mDifficultyMultiplier = 1.5;
			break;
		}
	}

	qDebug() << "Initialising game (Difficulty multiplier = " << mDifficultyMultiplier << ")";

	initialiseLevel(0);
}

void PlayScreen::initialiseLevel(quint32 level)
{
	qDebug() << "*** Initialising level " << level << " ***";

	hideAllObjects(false);

	mLevelCompleted = false;
	mReadyForPressAnyKey = false;

	mLevelCompleteText->setVisible(false);
	mTimeBonusText->setVisible(false);
	mPressAnyKeyText->setVisible(false);

	//Reset the camera
	cameraElevationAngle = 30.0;
	cameraRotationAngle = 0.0;
	cameraDistance = 145.0;

	mEnemyX = 16.0f;
	mEnemyY = 48.0f;
	mEnemyZ = 0.0f;

	mEnemyXDelta = 1;
	mEnemyZDelta = 0;

	mLastShotFiredTime = 0;
	mNextBombCountdown = 1.0f;

	mNextBomb = 0;

	//Enemies
	mEnemyRoot->setPosition(QVector3D(0.0f, 0.0f, 0.0f));
	fillEnemyArray(level);
	updateSurvivingEnemiesDetails();

	mTankObject->setPosition(currentLevelSettings().startPosition);

	QVector3D railgunPos = mRailgunObject->position();
	railgunPos.setX(mTankObject->position().x());
	railgunPos.setZ(mTankObject->position().z());
	mRailgunObject->setPosition(railgunPos);

	updateReadyToShootMarkers();
	

	mTankObject->mComponent->setEnabled(true);

	//Make sure no powerups are active
	clearActivePowerUp();
	//Make sure no power ups are carried across levels
	foreach(const PowerUp& powerUp, mPowerUpList)
	{
		powerUp.object->mComponent->setEnabled(false);
	}

	mPlayerState = PlayerStates::Alive;

	mEmptySphereList.clear();

	mClearWaitingPowerUpTimer.stop(); // Incase it's running from previous level.

	//Bit of a hack, but otherwise we pick up the mouse click from the menu.
	//Not sure why the keyboard one is required but fire gets stuck on otherwise after high score table.
	mouse->releaseAll();
	keyboard->releaseAll();

	//Reset timer
	mLevelStartedTime = currentTimeInSeconds;

	//Play music
	qApp->mMusicPlayer->playSong(currentLevelSettings().music);

	//If we are in demo mode and we've reached the fourth level then it's time to stop
	if((IsFullGame == false) && (level == 3))
	{
		QTimer::singleShot(2000, this, SLOT(demoFinished(void)));
	}
}

void PlayScreen::initialiseLevelSettings()
{
	int level;

	//Defaults
	for(level = 0; level < NoOfLevels; level++)
	{
		mLevelSettings[level].enemyMovementSpeed = 2.0f;
		mLevelSettings[level].enemyDropSpeed = 0.2f;
		mLevelSettings[level].timeBetweenBombs = 5.0f;
		mLevelSettings[level].validPowerUps.clear();
		mLevelSettings[level].music = "Gameplay1.ogg";
		mLevelSettings[level].targetTimeInSeconds = 90;
		mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 64.0f);
		//mLevelSettings[level].terrainType = TerrainTypes::Rocky;
		//mLevelSettings[level].terrainSeed = 1000;
	}
	
	//Level 0
	level = 0;
	mLevelSettings[level].enemyMovementSpeed = 2.0f;
	mLevelSettings[level].enemyDropSpeed = 0.2f;
	mLevelSettings[level].timeBetweenBombs = 2.1f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 180;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 1
	level = 1;
	mLevelSettings[level].enemyMovementSpeed = 2.5f;
	mLevelSettings[level].enemyDropSpeed = 0.2f;
	mLevelSettings[level].timeBetweenBombs = 1.9f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 240;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 2
	level = 2;
	mLevelSettings[level].enemyMovementSpeed = 3.0f;
	mLevelSettings[level].enemyDropSpeed = 0.2f;
	mLevelSettings[level].timeBetweenBombs = 1.7f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 3
	level = 3;
	mLevelSettings[level].enemyMovementSpeed = 3.5f;
	mLevelSettings[level].enemyDropSpeed = 0.25f;
	mLevelSettings[level].timeBetweenBombs = 1.5f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 4
	level = 4;
	mLevelSettings[level].enemyMovementSpeed = 4.0f;
	mLevelSettings[level].enemyDropSpeed = 0.3f;
	mLevelSettings[level].timeBetweenBombs = 1.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 5
	level = 5;
	mLevelSettings[level].enemyMovementSpeed = 4.5f;
	mLevelSettings[level].enemyDropSpeed = 0.35f;
	mLevelSettings[level].timeBetweenBombs = 1.1f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 6
	level = 6;
	mLevelSettings[level].enemyMovementSpeed = 5.0f;
	mLevelSettings[level].enemyDropSpeed = 0.4f;
	mLevelSettings[level].timeBetweenBombs = 0.9f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 7
	level = 7;
	mLevelSettings[level].enemyMovementSpeed = 5.5f;
	mLevelSettings[level].enemyDropSpeed = 0.45f;
	mLevelSettings[level].timeBetweenBombs = 0.7f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 8
	level = 8;
	mLevelSettings[level].enemyMovementSpeed = 6.0f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.5f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 1 (repeated)
	level = 9;
	mLevelSettings[level].enemyMovementSpeed = 6.5f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 2 (repeated)
	level = 10;
	mLevelSettings[level].enemyMovementSpeed = 7.0f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 3 (repeated)
	level = 11;
	mLevelSettings[level].enemyMovementSpeed = 7.5f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 4 (repeated)
	level = 12;
	mLevelSettings[level].enemyMovementSpeed = 8.0f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);

	//Level 5 (repeated)
	level = 13;
	mLevelSettings[level].enemyMovementSpeed = 8.5f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 6 (repeated)
	level = 14;
	mLevelSettings[level].enemyMovementSpeed = 9.0f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 7 (repeated)
	level = 15;
	mLevelSettings[level].enemyMovementSpeed = 9.5f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay2.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(32.0f, 4.0f, 96.0f);

	//Level 8 (repeated)
	level = 16;
	mLevelSettings[level].enemyMovementSpeed = 10.0f;
	mLevelSettings[level].enemyDropSpeed = 0.5f;
	mLevelSettings[level].timeBetweenBombs = 0.3f;
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::RapidFire);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Spray);
	mLevelSettings[level].validPowerUps.append(PowerUpTypes::Railgun);
	mLevelSettings[level].music = "Gameplay1.ogg";
	mLevelSettings[level].targetTimeInSeconds = 300;
	mLevelSettings[level].startPosition = QVector3D(64.0f, 4.0f, 96.0f);
}

void PlayScreen::update()
{
	GameScreen::update();

	updateCamera();	

	if((mReadyForPressAnyKey == false) && (mPaused == false))
	{
		updateBombs();
		updateMissiles();
		updateRailgun();
		updateFireballs();
		updatePowerUps();

		if((mGameOver == false) && (mLevelCompleted == false))
		{
			updateEnemyPositions();

			if((mouse->isPressed(Qt::LeftButton) || mouse->isPressed(Qt::MiddleButton) || mouse->isPressed(Qt::RightButton)
				|| (keyboard->isPressed(mTankWarsViewWidget->mVoxeliensSettings->value("input/fireKey", defaultFireVirtualKey()).toUInt())))
				&& (mPlayerState != PlayerStates::Dead))
			{		
				switch(mCurrentPowerUp)
				{			
				case PowerUpTypes::RapidFire:
					{
						if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShotsRapidFire)
						{
							Missile* missile = findDisabledMissile(PowerUpTypes::RapidFire);
							missile->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile->object->mComponent->setEnabled(true);
							missile->velocity = QVector3D(0.0f, missileSpeed, 0.0f);

							mLastShotFiredTime = currentTimeInSeconds;

							Mix_PlayChannel(-1, mMissileSound, 0);
						}
						break;
					}
				case PowerUpTypes::Spray:
					{
						if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
						{
							Missile* missile0 = findDisabledMissile(PowerUpTypes::Spray);
							missile0->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile0->object->mComponent->setEnabled(true);
							missile0->velocity = QVector3D(0.0f, missileSpeed, 0.0f);

							Missile* missile1 = findDisabledMissile(PowerUpTypes::Spray);
							missile1->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile1->object->mComponent->setEnabled(true);
							missile1->velocity = QVector3D(-sprayMissileHorzSpeed, missileSpeed, -sprayMissileHorzSpeed);

							Missile* missile2 = findDisabledMissile(PowerUpTypes::Spray);
							missile2->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile2->object->mComponent->setEnabled(true);
							missile2->velocity = QVector3D(-sprayMissileHorzSpeed, missileSpeed, sprayMissileHorzSpeed);

							Missile* missile3 = findDisabledMissile(PowerUpTypes::Spray);
							missile3->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile3->object->mComponent->setEnabled(true);
							missile3->velocity = QVector3D(sprayMissileHorzSpeed, missileSpeed, sprayMissileHorzSpeed);

							Missile* missile4 = findDisabledMissile(PowerUpTypes::Spray);
							missile4->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile4->object->mComponent->setEnabled(true);
							missile4->velocity = QVector3D(sprayMissileHorzSpeed, missileSpeed, -sprayMissileHorzSpeed);

							mLastShotFiredTime = currentTimeInSeconds;

							Mix_PlayChannel(-1, mMissileSound, 0);
						}
						break;
					}
				case PowerUpTypes::Railgun:
					{
						if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
						{
							mRailgunObject->mComponent->setEnabled(true); //Make sure it is visible;
							mLastShotFiredTime = currentTimeInSeconds;
							QTimer::singleShot(100, this, SLOT(finishFiringRailgun(void)));

							Mix_PlayChannel(-1, mRailgunSound, 0);
						}
						break;
					}
				case PowerUpTypes::Last:
					{
						if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
						{
							Missile* missile = findDisabledMissile(PowerUpTypes::Last);
							missile->object->setPosition(mTankObject->position() + QVector3D(0,0,0));
							missile->object->mComponent->setEnabled(true);
							missile->velocity = QVector3D(0.0f, missileSpeed, 0.0f);

							mLastShotFiredTime = currentTimeInSeconds;

							Mix_PlayChannel(-1, mMissileSound, 0);

							mLastFiredPosition = mTankObject->position();
						}
						break;
					}
				default:
					Q_ASSERT(false); //Should never hit this.
				}
			}

			//Check if we've finished the map
			if(mNoOfSurvivingEnemies == 0)
			{
				shutdownLevel();
			}

			float groundHeight = 4.0f;
			float fudgeFactor = 4.0; //Bounding box only encompasses centre of enemies, etc.
			if((mEnemyRoot->position().y() + mSurvivingEnemiesMinCorner.y() - fudgeFactor) < groundHeight)
			{
				gotoGameOverScreen(false);
			}
		}

		//Movement based on view direction
		QVector3D tankForwardDirection = cameraNode->zAxis();
		tankForwardDirection.setY(0.0f);
		tankForwardDirection.normalize();
		QVector3D tankRightDirection = cameraNode->xAxis();
		tankRightDirection.setY(0.0f);
		tankRightDirection.normalize();

		//This control scheme feels natural in that the tank moves towards and away from the camera. But
		//it is strange when you are facing the world head on, under an enemy at the side of the screen,
		//and you press 'up'. In this case you move away from the camera but not to underneath the next enemy.
		/*QVector3D tankForwardDirection = mTankObject->position() - QVector3D(mTankWarsViewWidget->mCameraSceneNode->getPosition().x, mTankWarsViewWidget->mCameraSceneNode->getPosition().y, mTankWarsViewWidget->mCameraSceneNode->getPosition().z);
		tankForwardDirection.setY(0.0f);
		tankForwardDirection.normalize();
		tankForwardDirection *= -1.0f;
		QVector3D tankRightDirection = cameraNode->xAxis();
		tankRightDirection.setY(0.0f);
		tankRightDirection.normalize();*/

		/*QVector3D tankRightDirection = QVector3D::crossProduct(tankForwardDirection, QVector3D(0.0f, 1.0f, 0.0f));
		tankRightDirection.setY(0.0f);
		tankRightDirection.normalize();
		tankRightDirection*= -1.0f;*/

		/*QVector3D tankForwardDirection;
		QVector3D tankRightDirection;

		qDebug() << "TPWS = " << mTankObject->position();
		Ogre::Vector2 tankPosScreenSpace = worldToScreen(Ogre::Vector4(mTankObject->position().x(), mTankObject->position().y(), mTankObject->position().z(), 1.0f), mTankWarsViewWidget->mOgreCamera);
		qDebug() << "TPSS = (" << tankPosScreenSpace.x << "," << tankPosScreenSpace.y << ")";
		//tankPosScreenSpace += Ogre::Vector2(0.0f, -0.01f);
		Ogre::Ray destRay = mTankWarsViewWidget->mOgreCamera->getCameraToViewportRay(tankPosScreenSpace.x, -tankPosScreenSpace.y);
		//Ogre::Ray destRay = mTankWarsViewWidget->mOgreCamera->getCameraToViewportRay(0.1f, 0.9f);
		//qDebug() << "Ray origin = (" << destRay.getOrigin().x << "," << destRay.getOrigin().y << "," << destRay.getOrigin().z << ")";
		//qDebug() << "Ray dir    = (" << destRay.getDirection().x << "," << destRay.getDirection().y << "," << destRay.getDirection().z << ")";
		Ogre::Plane groundPlane(0.0, -1.0, 0.0, 4.0);
		std::pair<bool, Ogre::Real> intersectionResult = destRay.intersects(groundPlane);
		if(intersectionResult.first)
		{
			Ogre::Vector3 intersection = destRay.getPoint(intersectionResult.second);

			qDebug() << "Intersect = (" << intersection.x << "," << intersection.y << "," << intersection.z << ")";

			tankForwardDirection = QVector3D(intersection.x, intersection.y, intersection.z);
			tankForwardDirection -= mTankObject->position();
			tankForwardDirection.normalize();

			tankRightDirection = QVector3D::crossProduct(tankForwardDirection, QVector3D(0.0f, 1.0f, 0.0f));
			tankRightDirection.setY(0.0f);
			tankRightDirection.normalize();
			tankRightDirection*= -1.0f;
		}*/

		if(mTankWarsViewWidget->mVoxeliensSettings->value("input/moveRelativeToWorld", false).toBool())
		{
			//Movement locked to world axes
			tankForwardDirection = QVector3D(0.0f, 0.0f, 1.0f);
			tankRightDirection   = QVector3D(1.0f, 0.0f, 0.0f);
		}

		float tankDistanceTravelled = TankSpeed * timeElapsedInSeconds;
		tankForwardDirection *= tankDistanceTravelled;
		tankRightDirection *= tankDistanceTravelled;

		QVector3D tankPos = mTankObject->position();
		float xMovement = 0.0f;
		float zMovement = 0.0f;

		if(keyboard->isPressed(Qt::Key_Up) || keyboard->isPressed(mTankWarsViewWidget->mVoxeliensSettings->value("input/forwardKey", defaultForwardVirtualKey()).toUInt()))
		{		
			xMovement -= tankForwardDirection.x();
			zMovement -= tankForwardDirection.z();
		}
		if(keyboard->isPressed(Qt::Key_Down) || keyboard->isPressed(mTankWarsViewWidget->mVoxeliensSettings->value("input/backKey", defaultBackVirtualKey()).toUInt()))
		{		
			xMovement += tankForwardDirection.x();
			zMovement += tankForwardDirection.z();
		}
		if(keyboard->isPressed(Qt::Key_Left) || keyboard->isPressed(mTankWarsViewWidget->mVoxeliensSettings->value("input/leftKey", defaultLeftVirtualKey()).toUInt()))
		{		
			xMovement -= tankRightDirection.x();
			zMovement -= tankRightDirection.z();
		}
		if(keyboard->isPressed(Qt::Key_Right) || keyboard->isPressed(mTankWarsViewWidget->mVoxeliensSettings->value("input/rightKey", defaultRightVirtualKey()).toUInt()))
		{		
			xMovement += tankRightDirection.x();
			zMovement += tankRightDirection.z();
		}

		QVector3D xMovementVector(xMovement, 0.0f, 0.0f);
		if(isPositionValidForTank(tankPos + xMovementVector))
		{
			tankPos += xMovementVector;
		}

		QVector3D zMovementVector(0.0f, 0.0f, zMovement);
		if(isPositionValidForTank(tankPos + zMovementVector))
		{
			tankPos += zMovementVector;
		}
		mTankObject->setPosition(tankPos);

		QVector3D railgunPos = mRailgunObject->position();
		railgunPos.setX(tankPos.x());
		railgunPos.setZ(tankPos.z());
		mRailgunObject->setPosition(railgunPos);

		updateReadyToShootMarkers();
	}

	mUpdateCounter++;
}

void PlayScreen::updateCamera(void)
{
	float wheelDelta = mouse->getWheelDelta();
	cameraDistance -= wheelDelta / 12; //10 units at a time.

	cameraDistance = qMin(cameraDistance, 200.0f);
	cameraDistance = qMax(cameraDistance, 91.0f); //sqrt(64*64+64*64) to stop camera clipping with volume


	cameraNode->setOrientation(QQuaternion());	
	cameraNode->yaw(-cameraRotationAngle);
	cameraNode->pitch(-cameraElevationAngle);

	cameraNode->setPosition(cameraFocusPoint);

	//Rotate tank with camera
	//mTankObject->setOrientation(QQuaternion());	
	//mTankObject->yaw(-cameraRotationAngle);

	mTankWarsViewWidget->mCamera->setOrientation(QQuaternion());
	mTankWarsViewWidget->mCamera->setPosition(QVector3D(0,0,cameraDistance));
}

void PlayScreen::updateFireballs(void)
{
	//Update the fireballs
	for(int ct = 0; ct < mFireballList.size(); ct++)
	{
		Fireball& fireball = mFireballList[ct];
		fireball.explosionAge += timeElapsedInSeconds;

		//Compute radius from volume
		float fireballVolume;
		if(fireball.isBig)
		{
			fireballVolume = fireball.explosionAge * 1000.0f;
		}
		else
		{
			fireballVolume = fireball.explosionAge * 40.0f;
		}
		float rCubed = (3.0*fireballVolume) / (4.0f * 3.142f);
		float r = qPow(rCubed, 1.0f/3.0f);

		float fireballRadius = r;
		if(fireballRadius > 0.001f)
		{
			fireball.object->mComponent->setEnabled(true);
			fireball.object->setSize(QVector3D(fireballRadius, fireballRadius, fireballRadius));
			fireball.explosionLight->setEnabled(true);

			float lightIntensity = fireball.explosionAge / (1.5f);
			lightIntensity = qMax(lightIntensity, 0.0f);
			lightIntensity = qMin(lightIntensity, 1.0f);

			lightIntensity = 1.0f - lightIntensity;

			lightIntensity *= lightIntensity;
			
			int red = qMin(255, static_cast<int>(255 * lightIntensity));
			int green = qMin(255, static_cast<int>(128 * lightIntensity));
			QColor explosionColour(red, green, 0);
			fireball.explosionLight->setColour(explosionColour);
		}

		if(fireball.isBig)
		{
			if(fireballRadius > 9.0f)
			{
				fireball.object->mComponent->setEnabled(false);
				fireball.explosionLight->setEnabled(false);
			}
		}
		else
		{
			if(fireballRadius > 3.0f)
			{
				fireball.object->mComponent->setEnabled(false);
				fireball.explosionLight->setEnabled(false);
			}
		}
	}
}

void PlayScreen::updatePowerUps(void)
{
	const float acceleration = 20.0;

	//Update the powerups
	for(int ct = 0; ct < mPowerUpList.size(); ct++)
	{
		PowerUp& powerUp = mPowerUpList[ct];

		if(powerUp.object->mComponent->isEnabled())
		{
			//Update the position of the powerup
			powerUp.velocity += timeElapsedInSeconds * acceleration;
			float distanceMoved = timeElapsedInSeconds * powerUp.velocity;
			QVector3D position = powerUp.object->position();
			position.setY(position.y() - distanceMoved);

			//Stop when it hits the ground
			const float powerUpGroundHeight = 5.0f;
			if(position.y() < powerUpGroundHeight)
			{
				powerUp.velocity = 0.0f;
				position.setY(powerUpGroundHeight);
			}

			//Set the position
			powerUp.object->setPosition(position);

			//Check if the player touches it
			QVector3D distanceToPlayer = powerUp.object->position() - mTankObject->derivedPosition();
			bool playerTouchesPowerUp = (distanceToPlayer.lengthSquared() < 49.0f) && (mPlayerState == PlayerStates::Alive || mPlayerState == PlayerStates::Respawning);
			if(playerTouchesPowerUp)
			{
				powerUp.object->mComponent->setEnabled(false);

				mCurrentPowerUp = powerUp.type;

				mClearActivePowerUpTimer.stop(); //We could already have a powerup so restart the timer
				mClearActivePowerUpTimer.setInterval(powerUpWearOffTimeInMs);	
				mClearActivePowerUpTimer.start();	

				qDebug() << "Collected powerup";

				Mix_PlayChannel(-1, mPowerupSound, 0);
			}
		}
	}
}
	
void PlayScreen::updateMissiles(void)
{
	//Move all the missiles forward a bit
	//float missileSpeed = 200.0f;
	//float missileDistance = timeElapsedInSeconds * missileSpeed;
	Missile missile;
	for(int i = 0; i < mMissileList.size(); i++)
	{
		missile = mMissileList.at(i);
		QVector3D missileDistance = timeElapsedInSeconds * missile.velocity;
		missile.object->setPosition(missile.object->position() + missileDistance);
		if(missile.object->position().y() > 200.0f)
		{
			missile.object->mComponent->setEnabled(false);
		}
	}			

	//Collision detection
	for(int i = 0; i < mMissileList.size(); i++)
	{
		missile = mMissileList.at(i);
		if(missile.object->mComponent->isEnabled())
		{
			//Collision with ground
			if(mTankWarsViewWidget->mVolume->materialAtPosition(missile.object->position()) != 0)
			{
				missile.object->mComponent->setEnabled(false);
				destroyGround(missile.object->derivedPosition());
			}

			//Collision with enemy
			for(int x = 0; x < 6; x++)
			{
				for(int y = 0; y < 3; y++)
				{
					for(int z = 0; z < 6;z++)
					{
						Thermite::Object* enemyObject = mEnemies[x][y][z];
						if(enemyObject)
						{
							if(enemyObject->mComponent->isEnabled())
							{
								QVector3D distance = missile.object->position() - enemyObject->derivedPosition();
								if(distance.lengthSquared() < 49.0f)
								{
									//Hit!									
									destroyEnemy(enemyObject, missile.type == PowerUpTypes::Spray);
									missile.object->mComponent->setEnabled(false);
								}
							}
						}
					}
				}
			}
		}
	}
}

void PlayScreen::updateRailgun(void)
{
	if(mRailgunObject->mComponent->isEnabled())
	{
		for(int x = 0; x < 6; x++)
		{
			for(int y = 0; y < 3; y++)
			{
				for(int z = 0; z < 6;z++)
				{
					Thermite::Object* enemyObject = mEnemies[x][y][z];
					if(enemyObject)
					{
						if(enemyObject->mComponent->isEnabled())
						{
							QVector3D distance = mRailgunObject->position() - enemyObject->derivedPosition();
							distance.setY(0.0f); //We're doing a 2D distance - only x and z
							if(distance.lengthSquared() < 25.0f)
							{
								//Hit!
								destroyEnemy(enemyObject, false);
							}
						}						
					}
				}
			}
		}

		//Create hole in ground
		int xRailgun = static_cast<int>(mRailgunObject->position().x());
		int yRailgun = static_cast<int>(mRailgunObject->position().y());
		int zRailgun = static_cast<int>(mRailgunObject->position().z());
		mTankWarsViewWidget->mVolume->createVerticalHole(xRailgun, 5, zRailgun, 31);
		mTankWarsViewWidget->mVolume->createVerticalHole(xRailgun+1, 5, zRailgun, 31);
		mTankWarsViewWidget->mVolume->createVerticalHole(xRailgun, 5, zRailgun+1, 31);
		mTankWarsViewWidget->mVolume->createVerticalHole(xRailgun+1, 5, zRailgun+1, 31);
	}
}

void PlayScreen::updateBombs(void)
{
	//Decrement the countdown
	mNextBombCountdown -= timeElapsedInSeconds;

	//If it's time then drop a new bomb
	if((mNextBombCountdown < 0.0f) && (mGameOver == false))
	{
		Thermite::Object* pBomber = 0;
		int attempts = 0;
		do
		{
			int xPos = qrand() % 6;
			int yPos = qrand() % 3;
			int zPos = qrand() % 6;

			if(mEnemies[xPos][yPos][zPos])
			{
				if(mEnemies[xPos][yPos][zPos]->mComponent->isEnabled())
				{
					pBomber = mEnemies[xPos][yPos][zPos];
				}
			}

			attempts++;
		}
		while((!pBomber) && (attempts < 10));

		if(pBomber)
		{
			Mix_PlayChannel(-1, mBombSound, 0);

			QVector3D bombOffset((++mBombOffset) % 5 - 2, 0.0, (++mBombOffset) % 5 - 2);

			Thermite::Object* bombObject = mBombList.at(mNextBomb);
			bombObject->setPosition( pBomber->derivedPosition() + bombOffset);
			bombObject->mComponent->setEnabled(true);
			++mNextBomb %= mBombList.count();

			mNextBombCountdown = currentLevelSettings().timeBetweenBombs / mDifficultyMultiplier;
		}
	}

	//Update the position of any existing bombs
	float bombSpeed = 50.0f;
	float bombDistance = timeElapsedInSeconds * bombSpeed;
	Thermite::Object* bombObject;
	for(int i = 0; i < mBombList.size(); i++)
	{
		bombObject = mBombList.at(i);
		bombObject->setPosition(bombObject->position() - QVector3D(0, bombDistance, 0));
		if(bombObject->position().y() < -100.0f)
		{
			bombObject->mComponent->setEnabled(false);
		}
	}

	//Collision detection		
	for(int i = 0; i < mBombList.size(); i++)
	{
		bombObject = mBombList.at(i);
		if(bombObject->mComponent->isEnabled())
		{
			QVector3D distanceToPlayer = bombObject->position() - mTankObject->derivedPosition();
			bool playerHit = (distanceToPlayer.lengthSquared() < 49.0f) && (mPlayerState == PlayerStates::Alive);
			if((mTankWarsViewWidget->mVolume->materialAtPosition(bombObject->position()) != 0) || //Ground hit
				( playerHit )) //or player hit
			{
				bombObject->mComponent->setEnabled(false);
				destroyGround(bombObject->derivedPosition());			
			}

			//We don't allow the player to die if they are on their last life and they get hit after they have destroyed the last enemy.
			//This is to avoid both the level being complete and the game also being over.
			if(playerHit && ((mLevelCompleted == false) || (mSpareLives > 0)))
			{
				mTankObject->mComponent->setEnabled(false);

				if(mSpareLives > 0)
				{
					//Use a life to respawn
					mSpareLives--;
					updateSpareLivesText();

					clearActivePowerUp();

					qDebug() << "Player hit! " << mSpareLives << " lives remaining.";

					mPlayerState = PlayerStates::Dead;
					QTimer::singleShot(2000, this, SLOT(startRespawn(void)));
					QTimer::singleShot(5000, this, SLOT(finishRespawn(void)));
				}
				else
				{
					qDebug() << "Player hit! Game over on level " << mCurrentLevel;
					gotoGameOverScreen(false);				
				}
			}
		}
	}
}

void PlayScreen::updateEnemyPositions(void)
{
	float enemyDistance = timeElapsedInSeconds * currentLevelSettings().enemyMovementSpeed * mDifficultyMultiplier;

	mEnemyX += mEnemyXDelta * enemyDistance;
	mEnemyZ += mEnemyZDelta * enemyDistance;

	if(mEnemyX + mSurvivingEnemiesMaxCorner.x() > 126)
	{
		mEnemyX--;
		mEnemyXDelta = 0;
		mEnemyZDelta = 1;
	}

	if(mEnemyZ + mSurvivingEnemiesMaxCorner.z() > 126)
	{
		mEnemyZ--;
		mEnemyXDelta = -1;
		mEnemyZDelta = 0;
	}

	if(mEnemyX + mSurvivingEnemiesMinCorner.x() < 2)
	{
		mEnemyX++;
		mEnemyXDelta = 0;
		mEnemyZDelta = -1;
	}

	if(mEnemyZ + mSurvivingEnemiesMinCorner.z() < 2)
	{
		mEnemyZ++;
		mEnemyXDelta = 1;
		mEnemyZDelta = 0;

		//And drop down
		//mEnemyY -= 4;
	}

	float enemyDropDistance = timeElapsedInSeconds * currentLevelSettings().enemyDropSpeed;
	mEnemyY -= enemyDropDistance;

	mEnemyRoot->setPosition(QVector3D(mEnemyX, mEnemyY, mEnemyZ));

	bool enemyCrashed = false;

	//Check for collisions
	for(int y = 0; y < 3; y++)
	{
		Thermite::Object* enemyObject = mEnemies[mColCheckLoopX][y][mColCheckLoopZ];
		if(enemyObject)
		{
			if(enemyObject->mComponent->isEnabled())
			{
				bool intersects = objectIntersectsVoxel(enemyObject);
				if(intersects)
				{
					enemyCrashed = true;

					enemyObject->mComponent->setEnabled(false);

					Mix_PlayChannel(-1, mExplosionSounds[mNextExplosionSound % NoOfExplosionSounds], 0);
					mNextExplosionSound++;

					//We create the crater below where the enemy's center was to destroy more ground for following enemies.
					QVector3D sphereOffset(mEnemyXDelta, 0.0f, mEnemyZDelta);
					sphereOffset *= 5.0f;
					sphereOffset.setY(-6.0f);

					Fireball* fireball = findDisabledFireball(true);
					fireball->object->setPosition(enemyObject->derivedPosition() + sphereOffset);
					fireball->explosionLightObject->setPosition(enemyObject->derivedPosition() + sphereOffset);
					fireball->explosionAge = 0.0;							
					fireball->object->mComponent->setEnabled(true);	

					mEmptySphereList.enqueue(EmptySphereData(enemyObject->derivedPosition() + sphereOffset, 12.0f));
					QTimer::singleShot(500, this, SLOT(createEmptySphereAt(void)));

					//We punish the player for a crash by subtracting some points... after all their planet has been pretty badly damaged!
					adjustScoreBy(-PointsLostPerCrash);
				}
			}
		}
	}

	mColCheckLoopX++;
	if(mColCheckLoopX == 6)
	{
		mColCheckLoopX = 0;
		mColCheckLoopZ++;
		mColCheckLoopZ %= 6;
	}

	if(enemyCrashed)
	{
		updateSurvivingEnemiesDetails();
	}
}

void PlayScreen::updateScoreText(void)
{
	QString scoreAsString;
	scoreAsString = QString("SCORE:%1").arg(QString::number(mScore), 6, QLatin1Char('0'));
	mScoreText->setText(scoreAsString);
}

void PlayScreen::updateSpareLivesText(void)
{
	QString spareLivesAsString;
	spareLivesAsString = QString("*%1").arg(QString::number(mSpareLives), 2, QLatin1Char('0'));
	mSpareLivesText->setText(spareLivesAsString);
}

void PlayScreen::updateReadyToShootMarkers(void)
{
	QVector3D readyToShootPos = mReadyToShootNormalObject->position();

	readyToShootPos.setX(mTankObject->position().x());
	readyToShootPos.setY(9.0f);
	readyToShootPos.setZ(mTankObject->position().z());

	mReadyToShootNormalObject->setPosition(readyToShootPos);
	mReadyToShootRapidFireObject->setPosition(readyToShootPos);
	mReadyToShootRailgunObject->setPosition(readyToShootPos);
	mReadyToShootSprayObject->setPosition(readyToShootPos);

	mReadyToShootNormalObject->mComponent->setEnabled(false);
	mReadyToShootRapidFireObject->mComponent->setEnabled(false);
	mReadyToShootRailgunObject->mComponent->setEnabled(false);
	mReadyToShootSprayObject->mComponent->setEnabled(false);

	bool isTankVisible = mTankObject->mComponent->isEnabled();

	switch(mCurrentPowerUp)
	{
	case PowerUpTypes::RapidFire:
		if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShotsRapidFire)
		{
			mReadyToShootRapidFireObject->mComponent->setEnabled(isTankVisible);
		}
		break;
	case PowerUpTypes::Railgun:
		if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
		{
			mReadyToShootRailgunObject->mComponent->setEnabled(isTankVisible);
		}
		break;
	case PowerUpTypes::Spray:
		if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
		{
			mReadyToShootSprayObject->mComponent->setEnabled(isTankVisible);
		}
		break;
	case PowerUpTypes::Last:
		if(currentTimeInSeconds > mLastShotFiredTime + delayBetweenShots)
		{
			mReadyToShootNormalObject->mComponent->setEnabled(isTankVisible);
		}
		break;
	}
}

void PlayScreen::shutdown(void)
{
	qDebug() << "PlayScreen::shutdown(void)";
	GameScreen::shutdown();

	clearEnemyArray();

	mFlashTankTimer.stop();

	mTankWarsViewWidget->unsetCursor();

	hideAllObjects();

	mGameOverText->setVisible(false);
	mDemoCompleteText->setVisible(false);
	mPressAnyKeyText->setVisible(false);
}

void PlayScreen::shutdownLevel(void)
{
	if(!mLevelCompleted) //Calling this multiple times has caused problems with Text3D::setText() function...
	{
		mLevelCompleted = true;
		mEmptySphereList.clear();

		QString levelAsString;
		if(mCurrentLevel < 16) //16 is last level, because we replay most of them
		{
			levelAsString = QString("LEVEL %1 COMPLETED").arg(QString::number(mCurrentLevel + 1), 2, QLatin1Char('0')); //Levels start at zero, but print them starting at 1.
		}
		else
		{
			levelAsString = QString("YOU BEAT THE GAME!"); //Levels start at zero, but print them starting at 1.
			mGameOver = true; //Set this so we go to high score table next.
		}
		mLevelCompleteText->setText(levelAsString);	
		mLevelCompleteText->setVisible(true);

		qint32 timeTaken = static_cast<quint32>(currentTimeInSeconds - mLevelStartedTime);
		qDebug() << "Level " << mCurrentLevel << " completed in " << timeTaken << " seconds";
		qint32 secondsToSpare = qMax((qint32)(currentLevelSettings().targetTimeInSeconds - timeTaken), (qint32)0);
		secondsToSpare /= mDifficultyMultiplier; //Otherwise harder settings are actually easier, as the enemies move into clear areas quicker.
		qint32 bonusPoints = secondsToSpare * PointsPerSpareSecond;
		adjustScoreBy(bonusPoints);

		QString timeBonusString = QString("TIME BONUS:%1").arg(QString::number(bonusPoints), 5, QLatin1Char('0'));
		mTimeBonusText->setText(timeBonusString);	
		mTimeBonusText->setVisible(true);

		mShowPressAnyKeyTimer.stop();
		mShowPressAnyKeyTimer.setInterval(PressAnyKeyDelayInMS);
		mShowPressAnyKeyTimer.setSingleShot(true);
		mShowPressAnyKeyTimer.start();

		clearEnemyArray();

		qApp->mMusicPlayer->fadeOffSong();

		//Clear any active powerups, so the power sown sounds
		//gets played now rather than when starting the new level.
		clearActivePowerUp();
	}
}

void PlayScreen::keyPressEvent(QKeyEvent* event)
{
	GameScreen::keyPressEvent(event);

	if(event->isAutoRepeat())
	{
		return;
	}

	if(mPaused)
	{		
		if(event->key() == Qt::Key_Escape)
		{			
			//Back to game
			mPaused = false;
			mQuitToMainMenuDialog->setVisible(false);
		}
		if(event->key() == Qt::Key_N)
		{
			//Back to game
			mPaused = false;
			mQuitToMainMenuDialog->setVisible(false);
		}
		if(event->key() == Qt::Key_Y)
		{
			mPaused = false;
			mQuitToMainMenuDialog->setVisible(false);

			//We could be on the 'level complete' screen.
			mShowPressAnyKeyTimer.stop();
			mLevelCompleteText->setVisible(false);
			mTimeBonusText->setVisible(false);
			mGameOverText->setVisible(false);
			mDemoCompleteText->setVisible(false);
			mPressAnyKeyText->setVisible(false);

			qApp->mMusicPlayer->fadeOffSong();
			mTankWarsViewWidget->setScreen(mTankWarsViewWidget->mMainMenuScreen);
		}
		
	}
	else
	{
		if(event->key() == Qt::Key_Escape)
		{
			qDebug() << "Escape pressed";
			mPaused = true;

			float windowAspect = static_cast<float>(mTankWarsViewWidget->m_pOgreRenderWindow->getWidth()) / static_cast<float>(mTankWarsViewWidget->m_pOgreRenderWindow->getHeight());
			mQuitToMainMenuDialog->setScale(1.0, windowAspect, 1.0);
			mQuitToMainMenuDialog->setVisible(true);
		}
		else
		{
			handlePressAnyKeyInput();
		}
	}
}

void PlayScreen::mousePressEvent(QMouseEvent* event)
{
	GameScreen::mousePressEvent(event);

	handlePressAnyKeyInput();
}

void PlayScreen::mouseMoveEvent(QMouseEvent* event)
{
	GameScreen::mouseMoveEvent(event);

	//Use of update counter is a dirty hack, as otherwise the
	//mouse doesn't get reset properly at the start of the game.
	if(event->spontaneous() && (mUpdateCounter > 10) && (mPaused == false))
	{
		//Rotate the camera
		float mouseDeltaX = event->pos().x() - mouse->previousPosition().x();
		float mouseDeltaY = event->pos().y() - mouse->previousPosition().y();

		float mouseSensitivity = mTankWarsViewWidget->mVoxeliensSettings->value("input/mouseSensitivity", 0.1f).toFloat();

		mouseDeltaX *= mouseSensitivity;
		mouseDeltaY *= mouseSensitivity;

		cameraRotationAngle += mouseDeltaX;			
		cameraElevationAngle += mouseDeltaY;

		cameraElevationAngle = qMin(cameraElevationAngle, 70.0f);
		cameraElevationAngle = qMax(cameraElevationAngle, -5.0f);

		//If movement is relative to world, then limit the rotation as it gets confusing at high rotations (reversed controls).
		if(mTankWarsViewWidget->mVoxeliensSettings->value("input/moveRelativeToWorld", false).toBool())
		{
			cameraRotationAngle = qMin(cameraRotationAngle, 35.0f);
			cameraRotationAngle = qMax(cameraRotationAngle, -35.0f);
		}

		//Reset the mouse cursor
		QRect geometry = mTankWarsViewWidget->geometry();
		QCursor::setPos((geometry.bottomRight() + geometry.topLeft()) * 0.5f);
	}
}

void PlayScreen::fillEnemyArray(quint32 level) //level should start at zero
{
	for(int x = 0; x < EnemyArrayWidth; x++)
	{
		for(int y = 0; y < EnemyArrayHeight; y++)
		{
			//Decide what type of enemy to spawn based on
			//it's height in the array and the current level.
			int enemyNumber = level + y - 2;
			enemyNumber %= NoOfEnemyTypes; 

			for(int z = 0; z < EnemyArrayDepth; z++)
			{
				if((enemyNumber > -1) /*&& (x >= 5) && (z >= 5)*/)
				{
					QString enemyNumberAsString = QString::number(enemyNumber);

					QVector3D spacing(EnemyHorzSpacing, EnemyVertSpacing, EnemyHorzSpacing);
					QVector3D offset = spacing / 2.0f;

					QVector3D pos(x,y,z);
					pos *= spacing;
					pos += offset;

					mEnemies[x][y][z] = new Thermite::Object(mEnemyRoot);
					Thermite::Entity* enemyEntity = new Thermite::Entity("NewEnemy" + enemyNumberAsString + ".mesh", "MeshMaterial", mEnemies[x][y][z]);
					mEnemies[x][y][z]->setPosition(pos);
					qApp->mObjectList.append(mEnemies[x][y][z]);
				}
				else
				{
					mEnemies[x][y][z] = 0;
				}
			}
		}
	}
}

void PlayScreen::clearEnemyArray(void)
{
	for(int x = 0; x < EnemyArrayWidth; x++)
	{
		for(int y = 0; y < EnemyArrayHeight; y++)
		{
			for(int z = 0; z < EnemyArrayDepth; z++)
			{
				Thermite::Object* enemyObject = mEnemies[x][y][z];

				if(enemyObject != 0)
				{
					qApp->mObjectList.removeOne(enemyObject);
					delete enemyObject;

					mEnemies[x][y][z] = 0;
				}
			}
		}
	}
}

void PlayScreen::updateSurvivingEnemiesDetails(void)
{
	int minX = 1000;
	int minY = 1000;
	int minZ = 1000;
	int maxX = -1000;
	int maxY = -1000;
	int maxZ = -1000;

	mNoOfSurvivingEnemies = 0;

	for(int x = 0; x < EnemyArrayWidth; x++)
	{
		for(int y = 0; y < EnemyArrayHeight; y++)
		{
			for(int z = 0; z < EnemyArrayDepth; z++)
			{
				if(mEnemies[x][y][z])
				{
					if(mEnemies[x][y][z]->mComponent->isEnabled())
					{
						minX = qMin(x, minX);
						minY = qMin(y, minY);
						minZ = qMin(z, minZ);

						maxX = qMax(x, maxX);
						maxY = qMax(y, maxY);
						maxZ = qMax(z, maxZ);

						mNoOfSurvivingEnemies++;
					}
				}
			}
		}
	}

	mSurvivingEnemiesMinCorner = QVector3D(minX, minY, minZ);
	mSurvivingEnemiesMaxCorner = QVector3D(maxX, maxY, maxZ);

	QVector3D enemySpacing(EnemyHorzSpacing, EnemyVertSpacing, EnemyHorzSpacing);

	mSurvivingEnemiesMinCorner *= enemySpacing;
	mSurvivingEnemiesMaxCorner *= enemySpacing;

	QVector3D offset = enemySpacing / 2.0f;
	mSurvivingEnemiesMinCorner += offset;
	mSurvivingEnemiesMaxCorner += offset;
}

bool PlayScreen::isPositionValidForTank(QVector3D desiredPosition)
{
	float tankSize = 3.0f;
	desiredPosition += QVector3D(0.5f, 0.5f, 0.5f); // So it will get rounded
	int32_t minX = desiredPosition.x() - tankSize;
	int32_t maxX = desiredPosition.x() + tankSize;
	int32_t minZ = desiredPosition.z() - tankSize;
	int32_t maxZ = desiredPosition.z() + tankSize;

	//Check it is within the map bounds
	if((desiredPosition.x() < -0.0f) || (desiredPosition.x() > 127.0f) || (desiredPosition.z() < 0.0f) || (desiredPosition.z() > 127.0f))
	{
		return false;
	}

	//Check all the voxels above the ground are empty.
	for(int32_t z = minZ; z <= maxZ; z++)
	{
		for(int32_t x = minX; x <= maxX; x++)
		{
			Vector3DInt32 offset(x,0,z);
			if(mTankWarsViewWidget->mVolume->m_pPolyVoxVolume->getVoxelAt(x, TankYPos, z).getMaterial() != 0)
			{
				return false;
			}
		}
	}

	return true;
}

void PlayScreen::startRespawn(void)
{
	mPlayerState = PlayerStates::Respawning;
}

void PlayScreen::finishRespawn(void)
{
	mPlayerState = PlayerStates::Alive;
	mTankObject->mComponent->setEnabled(true);
}

void PlayScreen::finishFiringRailgun(void)
{
	mRailgunObject->mComponent->setEnabled(false);
}

void PlayScreen::clearActivePowerUp(void)
{
	if(mCurrentPowerUp != PowerUpTypes::Last)
	{
		Mix_PlayChannel(-1, mPowerdownSound, 0);
	}

	mCurrentPowerUp = PowerUpTypes::Last;

	mClearActivePowerUpTimer.stop();
}

void PlayScreen::toggleTankVisible(void)
{
	if(mPlayerState == PlayerStates::Respawning)
	{
		//Toggle the tank
		mTankObject->mComponent->setEnabled(!mTankObject->mComponent->isEnabled());
	}
}

bool PlayScreen::objectIntersectsVoxel(Thermite::Object* object)
{
	Thermite::Entity* entity = dynamic_cast<Entity*>(object->mComponent);
	Ogre::AxisAlignedBox aabb = entity->mOgreEntity->getBoundingBox();

	//Iterate over the bottom of the box
	for(float z = aabb.getMinimum().z; z < aabb.getMaximum().z; z += 1.0f)
	{
		for(float x = aabb.getMinimum().x; x < aabb.getMaximum().x; x += 1.0f)
		{		
			QVector4D posToTest(x, aabb.getMinimum().y, z, 1.0f);
			QMatrix4x4 transform = object->transform();
			posToTest = transform * posToTest;

			Vector3DInt32 intPosToTest(posToTest.x() + 0.5f, posToTest.y() + 0.5f, posToTest.z() + 0.5f);
			Material16 voxel = mTankWarsViewWidget->mVolume->m_pPolyVoxVolume->getVoxelAt(intPosToTest);

			if((voxel.getMaterial() != 0) && (intPosToTest.getY() > 4)) //Test greater than four so we don't hit the ground.
			{
				//Collision
				return true;
			}
		}
	}
	
	//No collision
	return false;
}

void PlayScreen::hideAllObjects(bool includingStats)
{
	foreach(const Missile& missile, mMissileList)
	{
		missile.object->mComponent->setEnabled(false);
	}

	foreach(Thermite::Object* object, mBombList)
	{
		object->mComponent->setEnabled(false);
	}

	//Fireballs
	foreach(const Fireball& fireball, mFireballList)
	{
		fireball.object->mComponent->setEnabled(false);
		fireball.explosionLight->setColour(Qt::black);
		fireball.explosionLightObject->setPosition(QVector3D(10000.0f, 10000.0f, 10000.0f)); //Can't disable lights, so just move them away
	}

	//PowerUps
	foreach(const PowerUp& powerUp, mPowerUpList)
	{
		powerUp.object->mComponent->setEnabled(false);
	}

	//Tank
	mTankObject->mComponent->setEnabled(false);

	//These will all become invisible because the tank is hidden.
	updateReadyToShootMarkers();

	if(includingStats)
	{
		//Score text
		mScoreText->setVisible(false);

		//Life entity
		mLifeObject->mComponent->setEnabled(false);

		//Spare lives counter
		mSpareLivesText->setVisible(false);
	}
}

void PlayScreen::hideWaitingPowerUps(void)
{
	foreach(const PowerUp& powerUp, mPowerUpList)
	{
		if(powerUp.object->mComponent->isEnabled())
		{
			qDebug() << "Failed to collect powerup";
		}
		powerUp.object->mComponent->setEnabled(false);
	}
}

Fireball* PlayScreen::findDisabledFireball(bool big)
{
	Q_ASSERT(mFireballList.size() < 16); //Pretty sure we'll never need this many

	for(int ct = 0; ct < mFireballList.size(); ct++)
	{
		if(mFireballList[ct].object->mComponent->isEnabled() == false)
		{
			Thermite::Entity* fireballEntity = dynamic_cast<Thermite::Entity*>(mFireballList[ct].object->mComponent);
			if(big)
			{				
				fireballEntity->setMaterialName("FireballBigMaterial");
				mFireballList[ct].isBig = true;
			}
			else
			{
				fireballEntity->setMaterialName("FireballMaterial");
				mFireballList[ct].isBig = false;
			}
			return &(mFireballList[ct]);
		}
	}

	//free fireball not found - return a new one.
	Fireball fireball;
	fireball.object = new Object();
	Thermite::Entity* fireballEntity;
	if(big)
	{				
		fireballEntity = new Thermite::Entity("Icosphere5.mesh", "FireballBigMaterial", fireball.object);
		fireball.isBig = true;
	}
	else
	{
		fireballEntity = new Thermite::Entity("Icosphere5.mesh", "FireballMaterial", fireball.object);
		fireball.isBig = false;
	}
	fireballEntity->setCastsShadows(false);
	qApp->mObjectList.append(fireball.object);

	fireball.explosionLightObject = new Object();
	qApp->mObjectList.append(fireball.explosionLightObject);

	fireball.explosionLight = new Thermite::Light(fireball.explosionLightObject);
	fireball.explosionLight->setType(Thermite::Light::PointLight);

	mFireballList.append(fireball);
	return &(mFireballList[mFireballList.size()-1]);
}

PowerUp* PlayScreen::findDisabledPowerUp(PowerUpType type)
{
	Q_ASSERT(mPowerUpList.size() < 16); //Pretty sure we'll never need this many

	for(int ct = 0; ct < mPowerUpList.size(); ct++)
	{
		if((mPowerUpList[ct].object->mComponent->isEnabled() == false) && (mPowerUpList[ct].type == type))
		{
			return &(mPowerUpList[ct]);
		}
	}

	//free powerup not found - return a new one.
	PowerUp powerUp;
	powerUp.object = new Object();
	powerUp.type = type;
	Thermite::Entity* powerUpEntity;
	switch(type)
	{
	case PowerUpTypes::RapidFire:
		powerUpEntity = new Thermite::Entity("OrangePowerUp.mesh", "MeshMaterial", powerUp.object);
		break;
	case PowerUpTypes::Railgun:
		powerUpEntity = new Thermite::Entity("PurplePowerUp.mesh", "MeshMaterial", powerUp.object);
		break;
	case PowerUpTypes::Spray:
		powerUpEntity = new Thermite::Entity("CyanPowerUp.mesh", "MeshMaterial", powerUp.object);
		break;
	}
	qApp->mObjectList.append(powerUp.object);

	mPowerUpList.append(powerUp);
	return &(mPowerUpList[mPowerUpList.size()-1]);
}

Missile* PlayScreen::findDisabledMissile(PowerUpType type)
{
	Q_ASSERT(mMissileList.size() < 50); //Pretty sure we'll never need this many

	for(int ct = 0; ct < mMissileList.size(); ct++)
	{
		if((mMissileList[ct].object->mComponent->isEnabled() == false) && (mMissileList[ct].type == type))
		{
			return &(mMissileList[ct]);
		}
	}

	//free missile not found - return a new one.
	Missile missile;
	missile.object = new Object();
	missile.type = type;
	Thermite::Entity* missileEntity;
	switch(type)
	{
	case PowerUpTypes::RapidFire:
		missileEntity = new Thermite::Entity("OrangeMissile.mesh", "MeshMaterial", missile.object);
		break;
	/*case PowerUpTypes::Railgun: //This isn't a missile, it's a solid line
		missileEntity = new Thermite::Entity("PurpleMissile.mesh", "MeshMaterial", missile.object);
		break;*/
	case PowerUpTypes::Spray:
		missileEntity = new Thermite::Entity("CyanMissile.mesh", "MeshMaterial", missile.object);
		break;
	case PowerUpTypes::Last:
		missileEntity = new Thermite::Entity("WhiteMissile.mesh", "MeshMaterial", missile.object);
		break;
	}
	missileEntity->setCastsShadows(false);
	missile.object->lookAt(missile.object->position() + QVector3D(0,100,0));
	qApp->mObjectList.append(missile.object);

	mMissileList.append(missile);
	return &(mMissileList[mMissileList.size()-1]);
}

void PlayScreen::destroyEnemy(Thermite::Object* hitEnemy, bool destroyedBySpray)
{
	hitEnemy->mComponent->setEnabled(false);

	Mix_PlayChannel(-1, mExplosionSounds[mNextExplosionSound % NoOfExplosionSounds], 0);
	mNextExplosionSound++;

	Fireball* fireball = findDisabledFireball(false);
	fireball->object->setPosition(hitEnemy->derivedPosition());
	fireball->explosionLightObject->setPosition(hitEnemy->derivedPosition());
	fireball->explosionAge = 0.0;
	fireball->object->mComponent->setEnabled(true);

	//Spawn a powerup?
	int enabledPowerUps = 0;
	foreach(const PowerUp& powerUp, mPowerUpList)
	{
		if(powerUp.object->mComponent->isEnabled())
		{
			enabledPowerUps++;
		}
	}
	if((qrand() % powerUpSpawnFrequency == 0) && (currentLevelSettings().validPowerUps.isEmpty() == false) && (enabledPowerUps < 1) && (mCurrentPowerUp == PowerUpTypes::Last) && (!destroyedBySpray))
	{
		mClearWaitingPowerUpTimer.stop();

		PowerUpType powerUpType = PowerUpTypes::Last;
		do
		{
			powerUpType = static_cast<PowerUpType>(qrand() % static_cast<int>(PowerUpTypes::Last));
		} while(currentLevelSettings().validPowerUps.contains(powerUpType) == false);

		PowerUp* powerUp = findDisabledPowerUp(powerUpType);
		
		QVector3D position = hitEnemy->derivedPosition();

		//Prevent powerups falling outside accesible area.
		position.setX(mLastFiredPosition.x());
		position.setZ(mLastFiredPosition.z());

		powerUp->object->setPosition(position);

		powerUp->velocity = 0.0f;
		powerUp->hasLanded = false;
		powerUp->object->mComponent->setEnabled(true);

		//NOTE: All powerups will be cleared... this only works properly if we are only allowing one visible powerup at a time.	
		mClearWaitingPowerUpTimer.setInterval(powerUpWaitingTimeInMs);
		mClearWaitingPowerUpTimer.start();	
	}

	adjustScoreBy(50);

	updateSurvivingEnemiesDetails();
}

void PlayScreen::destroyGround(const QVector3D& position)
{
	QVector3D bombOffset(0.0,4.0,0.0); //Centre explosion above ground level

	Fireball* fireball = findDisabledFireball(false);
	fireball->object->setPosition(position + bombOffset);
	fireball->explosionLightObject->setPosition(position + bombOffset);
	fireball->explosionAge = 0.0;
	fireball->object->mComponent->setEnabled(true);

	Mix_PlayChannel(-1, mExplosionSounds[mNextExplosionSound % NoOfExplosionSounds], 0);
	mNextExplosionSound++;

	//mTankWarsViewWidget->mVolume->createSphereAt(position, 3.0f, 0, false);
	mEmptySphereList.enqueue(EmptySphereData(position, 3.0f));
	QTimer::singleShot(500, this, SLOT(createEmptySphereAt(void)));
}

const LevelSettings& PlayScreen::currentLevelSettings(void)
{
	return mLevelSettings[mCurrentLevel];
}

Ogre::Vector2 PlayScreen::worldToScreen(const Ogre::Vector4& worldPoint, Ogre::Camera* cam)
{
    // Pass point through camera projection matrices
    Ogre::Vector4 screenPoint = cam->getProjectionMatrix() *
                          cam->getViewMatrix() *
                          worldPoint;

    // Convert to relative screen space
    return Ogre::Vector2(screenPoint.x * 0.5f / screenPoint.w + 0.5f,
                   screenPoint.y * 0.5f / screenPoint.w + 0.5f);
}

void PlayScreen::showPressAnyKey(void)
{
	//Reset camera
	cameraElevationAngle = 30.0;
	cameraRotationAngle = 0.0;
	cameraDistance = 145.0;

	mPressAnyKeyText->setVisible(true);
	clearEnemyArray(); //Make sure enemies can't hide 'game over' text.
	mReadyForPressAnyKey = true;
}

void PlayScreen::createEmptySphereAt(void)
{
	//mTankWarsViewWidget->mVolume->createSphereAt(QVector3D(x,y,z), radius, 0, false);
	if(!mEmptySphereList.isEmpty())
	{
		EmptySphereData data = mEmptySphereList.dequeue();
		mTankWarsViewWidget->mVolume->createSphereAt(data.position, data.radius, 0, false);
	}
}

void PlayScreen::gotoGameOverScreen(bool bIsEndOfDemo)
{
	if(!mGameOver) //Multiple calls have seen problems with setText().
	{
		mGameOver = true;

		if(bIsEndOfDemo)
		{
			clearEnemyArray(); //So they can't hide the demo text.
			mDemoCompleteText->setVisible(true);
		}
		else
		{
			mGameOverText->setVisible(true);
		}

		mShowPressAnyKeyTimer.stop();
		mShowPressAnyKeyTimer.setInterval(PressAnyKeyDelayInMS);
		mShowPressAnyKeyTimer.setSingleShot(true);
		mShowPressAnyKeyTimer.start();

		qApp->mMusicPlayer->fadeOffSong();
	}
}

void PlayScreen::handlePressAnyKeyInput(void)
{
	if(mGameOver && mReadyForPressAnyKey)
	{
		mTankWarsViewWidget->setScreen(mTankWarsViewWidget->mHighScoreScreen);
		return;
	}

	if(mLevelCompleted && mReadyForPressAnyKey)
	{
		//Key press means start next level
		mCurrentLevel++;
		//generateMap(currentLevelSettings().terrainType, currentLevelSettings().terrainSeed, mTankWarsViewWidget->mVolume);

		int levelToLoad = mCurrentLevel;
		if(levelToLoad >= 9)
		{
			levelToLoad++; //We don't play the first level again as it's boring
			levelToLoad = levelToLoad % 9; //Repeat levels so the game lasts longer.
		}

		QString filename = QString(qApp->mResourcePath+"volumes/level%1.vol").arg(levelToLoad);
		mTankWarsViewWidget->mVolume->readFromFile(filename);

		initialiseLevel(mCurrentLevel);
	}
}

void PlayScreen::adjustScoreBy(int amount)
{
	mScore += amount;
	mScore = qMax(mScore, 0);
	updateScoreText();

	if(mScore >= mNextExtraLifeTarget)
	{
		mSpareLives++;
		updateSpareLivesText();
		mNextExtraLifeTarget += PointsPerExtraLife;
		qDebug() << "Gained extra life. Now has " << mSpareLives << " spare lives.";
	}
}

void PlayScreen::demoFinished(void)
{
	//Finish the demo
	gotoGameOverScreen(true);
}