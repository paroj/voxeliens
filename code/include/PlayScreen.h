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

#ifndef PLAYSCREEN_H_
#define PLAYSCREEN_H_

#include "GameScreen.h"
#include "TerrainGeneration.h"
#include "Text3D.h"

#include "PolyVoxCore/Vector.h"

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

#include <QTimer>
#include <QQueue>

#include <SDL.h>
#include <SDL_mixer.h>

#include "OgreCamera.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreSceneNode.h"

namespace PlayerStates
{
	enum PlayerState
	{
		Alive,
		Dead,
		Respawning
	};
}
typedef PlayerStates::PlayerState PlayerState;

struct Fireball
{
	Thermite::Object* object;
	Thermite::Object* explosionLightObject;
	Thermite::Light* explosionLight;
	float explosionAge;
	bool isBig;
};

struct EmptySphereData
{
	EmptySphereData(const QVector3D& position, float radius)
	{
		this->position = position;
		this->radius = radius;
	}

	QVector3D position;
	float radius;
};

namespace PowerUpTypes
{
	enum PowerUpType
	{
		Railgun,
		RapidFire,
		Spray,		
		Last
	};
}
typedef PowerUpTypes::PowerUpType PowerUpType;

struct PowerUp
{
	Thermite::Object* object;
	PowerUpType type;
	float velocity;
	bool hasLanded;
};

struct Missile
{
	Thermite::Object* object;
	PowerUpType type;
	QVector3D velocity;
};

struct LevelSettings
{
	float enemyMovementSpeed;
	float enemyDropSpeed;
	float timeBetweenBombs;
	QList<PowerUpType> validPowerUps;
	QString music;
	quint32 targetTimeInSeconds;
	QVector3D startPosition;
	TerrainType terrainType;
	int terrainSeed;
};

class PlayScreen : public GameScreen
{
	Q_OBJECT

	const static int EnemyArrayWidth = 6;
	const static int EnemyArrayHeight = 3;
	const static int EnemyArrayDepth = 6;
	const static int NoOfEnemyTypes = 6;
	const static int EnemyHorzSpacing = 16;
	const static int EnemyVertSpacing = 13;
	const static int MaxNoOfMissiles = 10;
	const static int MaxNoOfBombs = 10;
	const static int NoOfJoiningDots = 8;
	const static int NoOfLevels = 17;
	const static int NoOfExplosionSounds = 2;

public:
	PlayScreen(TankWarsViewWidget* tankWarsViewWidget);
	~PlayScreen();

	//Initialise
	virtual void initialise();
	void initialiseLevel(quint32 level);
	void initialiseLevelSettings();

	//Update
	virtual void update();
	void updateCamera(void);
	void updateBombs(void);
	void updateFireballs(void);
	void updatePowerUps(void);
	void updateMissiles(void);
	void updateRailgun(void);
	void updateEnemyPositions(void);
	void updateScoreText(void);
	void updateSpareLivesText(void);
	void updateReadyToShootMarkers(void);

	//Shutdown
	virtual void shutdown();	
	void shutdownLevel(void);

	//Input handling
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

	//Enemy array functions
	void fillEnemyArray(quint32 level);
	void clearEnemyArray(void);
	void updateSurvivingEnemiesDetails(void);

	//Utility functions
	bool isPositionValidForTank(QVector3D desiredPosition);
	bool objectIntersectsVoxel(Thermite::Object* object);
	void hideAllObjects(bool includingStats = true);
	Fireball* findDisabledFireball(bool big);
	PowerUp* findDisabledPowerUp(PowerUpType type);
	Missile* findDisabledMissile(PowerUpType type);
	void destroyEnemy(Thermite::Object* hitEnemy, bool destroyedBySpray);
	void destroyGround(const QVector3D& position);
	const LevelSettings& currentLevelSettings(void);
	Ogre::Vector2 worldToScreen(const Ogre::Vector4& worldPoint, Ogre::Camera* cam);
	void gotoGameOverScreen(bool bIsEndOfDemo);
	void handlePressAnyKeyInput(void);
	void adjustScoreBy(int amount);

public slots:
	void startRespawn(void);
	void finishRespawn(void);
	void toggleTankVisible(void);
	void clearActivePowerUp(void);
	void finishFiringRailgun(void);
	void showPressAnyKey(void);
	void createEmptySphereAt(void);
	void hideWaitingPowerUps(void);
	void demoFinished(void);

public:

	//Enemies
	Thermite::Object* mEnemyRoot;
	Thermite::Object* mEnemies[EnemyArrayWidth][EnemyArrayHeight][EnemyArrayDepth];
	QVector3D mSurvivingEnemiesMinCorner;
	QVector3D mSurvivingEnemiesMaxCorner;
	int mNoOfSurvivingEnemies;
	float mEnemyX;
	float mEnemyY;
	float mEnemyZ;
	int mEnemyXDelta;
	int mEnemyZDelta;

	//Score
	int32_t mScore;
	Text3D* mScoreText;

	//Lives
	//static const quint32 MaxSpareLives = 3;
	quint32 mSpareLives;
	Thermite::Object* mLifeObject;
	Thermite::Entity* mLifeEntity;
	Text3D* mSpareLivesText;

	//Fireballs
	QList<Fireball> mFireballList;

	//Tank
	Thermite::Object* mTankObject;
	Thermite::Entity* mTankEntity;

	//3D mouse cursor

	//Bombs and missiles
	QList<Missile> mMissileList;
	int mNextBomb;
	QList<Thermite::Object*> mBombList;

	//Railgun
	Thermite::Object* mRailgunObject;

	//Ready to shoot markers
	Thermite::Object* mReadyToShootNormalObject;
	Thermite::Object* mReadyToShootRapidFireObject;
	Thermite::Object* mReadyToShootRailgunObject;
	Thermite::Object* mReadyToShootSprayObject;

	//Powerups
	QVector<PowerUp> mPowerUpList;
	PowerUpType mCurrentPowerUp;

	//Other
	float mNextBombCountdown;
	float mLastShotFiredTime;
	bool mPathIsValid;
	float mLevelStartedTime;

	//Audio
	Mix_Chunk *mMissileSound;
	//Mix_Chunk *mExplosionSound;
	quint32 mNextExplosionSound;
	Mix_Chunk* mExplosionSounds[NoOfExplosionSounds];
	Mix_Chunk *mBombSound;
	Mix_Chunk *mRailgunSound;
	Mix_Chunk *mPowerupSound;
	Mix_Chunk *mPowerdownSound;

	Text3D* mLevelCompleteText;
	Text3D* mTimeBonusText;
	Text3D* mGameOverText;
	Text3D* mDemoCompleteText;
	Text3D* mPressAnyKeyText;

	//To hide the mouse cursor
	QCursor* mBlankCursor;
	bool mCursorHidden;

	PlayerState mPlayerState;

	QTimer mFlashTankTimer;
	QTimer mClearActivePowerUpTimer;
	QTimer mClearWaitingPowerUpTimer;
	QTimer mShowPressAnyKeyTimer;

	int mColCheckLoopX;
	int mColCheckLoopZ;

	int mCurrentLevel;
	bool mLevelCompleted;
	bool mGameOver;

	quint32 mUpdateCounter;

	LevelSettings mLevelSettings[NoOfLevels];

	QQueue<EmptySphereData> mEmptySphereList;

	QVector3D mLastFiredPosition;

	qint32 mNextExtraLifeTarget;

	bool mPaused;
	int mBombOffset; //Leave signed or we have problems with %

	float mDifficultyMultiplier;

	Ogre::SceneNode* mQuitToMainMenuDialog;
};

#endif //PLAYSCREEN_H_
