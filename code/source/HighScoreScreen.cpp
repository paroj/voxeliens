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

#include "HighScoreScreen.h"

#include <QDebug>
#include <QMouseEvent>
#include <qmath.h>
#include <QSettings>

#include "Mouse.h"
#include "TankWarsApplication.h"
#include "TankWarsViewWidget.h"

#ifndef _WIN32
	// We don't define this on Windows, as MSVC incorrectly treats the declaration
	// as a definition which results in multiply defined symbol errors.
	// See http://stackoverflow.com/a/3460227/849083 for more information
	const int HighScoreScreen::NoOfScores;
#endif

bool compareScores(const HighScore &s1, const HighScore &s2)
{
	//Sort backwards (high to low)
	return s1.score > s2.score;
}

HighScoreScreen::HighScoreScreen(TankWarsViewWidget* tankWarsViewWidget)
	:GameScreen(tankWarsViewWidget)
{	
}

void HighScoreScreen::initialise()
{
	qDebug() << "HighScoreScreen::initialise()";
	GameScreen::initialise();

	//The title text
	mTitleText = new Text3D();
	mTitleText->setPosition(QVector3D(25, 90, 64));
	mTitleText->setText("HIGH SCORES");
	mTitleText->setVisible(false);

	mNewHighScoreText = new Text3D();
	mNewHighScoreText->setPosition(QVector3D(11, 100, 64));
	mNewHighScoreText->setText("NEW HIGH SCORE!");
	mNewHighScoreText->setVisible(false);

	mEnterYourNameText = new Text3D();
	mEnterYourNameText->setPosition(QVector3D(11, 90, 64));
	mEnterYourNameText->setText("ENTER YOUR NAME");
	mEnterYourNameText->setVisible(false);

	mPressAnyKey = new Text3D();
	mPressAnyKey->setPosition(QVector3D(18, 25, 64));
	mPressAnyKey->setText("PRESS ANY KEY");
	mPressAnyKey->setVisible(false);

	for(int ct = 0; ct < NoOfScores; ct++)
	{
		//Name
		mPlayerNameText[ct] = new Text3D();
		mPlayerNameText[ct]->setPosition(QVector3D(11, 75 - ct * 10, 64));
		mPlayerNameText[ct]->setVisible(false);

		mPlayerScoreText[ct] = new Text3D();
		mPlayerScoreText[ct]->setPosition(QVector3D(74, 75 - ct * 10, 64));
		mPlayerScoreText[ct]->setVisible(false);
	}

	//Start by hiding everything
	mTitleText->setVisible(false);
	mNewHighScoreText->setVisible(false);
	mEnterYourNameText->setVisible(false);
	mPressAnyKey->setVisible(false);

	for(int ct = 0; ct < NoOfScores; ct++)
	{
		mPlayerNameText[ct]->setVisible(false);
		mPlayerScoreText[ct]->setVisible(false);
	}

	//Player name starts as empty string
	mPlayerName = "";

	//Get the current score
	quint32 currentScore = mTankWarsViewWidget->mPlayScreen->mScore;

	//Get the existing high scores and copy them into our list
	highScores.clear();
	HighScore highScore;
	highScore.name = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore0/name", "........").toString();
	highScore.score = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore0/score", 50000).toUInt();
	highScores.append(highScore);
	highScore.name = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore1/name", "........").toString();
	highScore.score = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore1/score", 40000).toUInt();
	highScores.append(highScore);
	highScore.name = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore2/name", "........").toString();
	highScore.score = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore2/score", 30000).toUInt();
	highScores.append(highScore);
	highScore.name = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore3/name", "........").toString();
	highScore.score = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore3/score", 20000).toUInt();
	highScores.append(highScore);
	highScore.name = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore4/name", "........").toString();
	highScore.score = mTankWarsViewWidget->mVoxeliensSettings->value("HighScore4/score", 10000).toUInt();
	highScores.append(highScore);

	//Sort the highscores high to low
	qStableSort(highScores.begin(), highScores.end(), compareScores);

	//Find the lowest existing highscore
	qint32 lowestHighScore = -1;
	if(highScores.size() > 0)
	{
		lowestHighScore = highScores.at(highScores.size() - 1).score;
	}

	qint32 currentScoreAsInt = (qint32)currentScore;

	mScoreIndex = -1;

	//Check if we made it onto the high score table
	if(currentScoreAsInt > lowestHighScore)
	{
		//Check if someone gets pushed off the bottom
		if(highScores.size() >= NoOfScores)
		{
			highScores.removeLast();
		}

		//Insert a new entry for our new score
		HighScore highScore;
		highScore.name = "";
		highScore.score = currentScore;
		highScores.append(highScore);

		//Redo the sort to put ourself in the correct position.
		qStableSort(highScores.begin(), highScores.end(), compareScores);

		//Find the index where our score ended up
		for(int ct = 0; ct < highScores.size(); ct++)
		{
			//Odds are pretty slim that an existing high score is the same as our and has a blank name
			if((highScores.at(ct).name.size() == 0) && (highScores.at(ct).score == currentScore))
			{
				mScoreIndex = ct;
				break;
			}
		}

		//New high score so so the appropriate text
		mNewHighScoreText->setVisible(true);
		mEnterYourNameText->setVisible(true);
		mEnteringName = true;
	}
	else
	{
		//Not a new high score
		mTitleText->setVisible(true);

		mReadyForPressAnyKey = false;
		QTimer::singleShot(1000, this, SLOT(showPressAnyKey(void)));

		mEnteringName = false;
	}

	//Show the scores
	for(int ct = 0; ct < NoOfScores; ct++)
	{
		if(ct < qMin(highScores.size(), NoOfScores))
		{			
			mPlayerNameText[ct]->setText(highScores.at(ct).name);	
			QString scoreAsString = QString("%1").arg(QString::number(highScores.at(ct).score), 6, QLatin1Char('0'));
			mPlayerScoreText[ct]->setText(scoreAsString);

			mPlayerNameText[ct]->setVisible(true);
			mPlayerScoreText[ct]->setVisible(true);
		}
	}

	//Play music
	qApp->mMusicPlayer->playSong("Highscores.ogg");

	cameraFocusPoint = QVector3D(64, 40, 64);
}

void HighScoreScreen::update()
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

void HighScoreScreen::shutdown()
{
	qDebug() << "HighScoreScreen::shutdown()";
	GameScreen::shutdown();

	//Hide the title
	mTitleText->setVisible(false);
	mNewHighScoreText->setVisible(false);
	mEnterYourNameText->setVisible(false);
	mPressAnyKey->setVisible(false);

	//Hide the scores
	for(int ct = 0; ct < NoOfScores; ct++)
	{
		mPlayerNameText[ct]->setVisible(false);
		mPlayerScoreText[ct]->setVisible(false);
	}

	//Clear the existing saved scores
	//QSettings highscoreSettings("./materials/VoxelMaterial/Mesh/Mesh.material.backup", QSettings::IniFormat);
	//highscoreSettings.clear();

	//Save the new high scores
	/*HighScore highScore;
	for(int ct = 0; ct < qMin(highScores.size(), NoOfScores); ct++)
	{
		//We use a seperate group for each player, which allows us to have duplicate or empty names
		highscoreSettings.beginGroup("HighScore" + QString::number(ct));

		if(ct == mScoreIndex)
		{
			highscoreSettings.setValue("name", mPlayerName);
		}
		else
		{
			highscoreSettings.setValue("name", highScores[ct].name);
		}

		highscoreSettings.setValue("score", highScores[ct].score);

		highscoreSettings.endGroup();
	}*/

	mTankWarsViewWidget->mVoxeliensSettings->beginGroup("HighScore0");
	mTankWarsViewWidget->mVoxeliensSettings->setValue("name", 0 == mScoreIndex ? mPlayerName : highScores[0].name);
	mTankWarsViewWidget->mVoxeliensSettings->setValue("score", highScores[0].score);
	mTankWarsViewWidget->mVoxeliensSettings->endGroup();

	mTankWarsViewWidget->mVoxeliensSettings->beginGroup("HighScore1");
	mTankWarsViewWidget->mVoxeliensSettings->setValue("name", 1 == mScoreIndex ? mPlayerName : highScores[1].name);
	mTankWarsViewWidget->mVoxeliensSettings->setValue("score", highScores[1].score);
	mTankWarsViewWidget->mVoxeliensSettings->endGroup();

	mTankWarsViewWidget->mVoxeliensSettings->beginGroup("HighScore2");
	mTankWarsViewWidget->mVoxeliensSettings->setValue("name", 2 == mScoreIndex ? mPlayerName : highScores[2].name);
	mTankWarsViewWidget->mVoxeliensSettings->setValue("score", highScores[2].score);
	mTankWarsViewWidget->mVoxeliensSettings->endGroup();

	mTankWarsViewWidget->mVoxeliensSettings->beginGroup("HighScore3");
	mTankWarsViewWidget->mVoxeliensSettings->setValue("name", 3 == mScoreIndex ? mPlayerName : highScores[3].name);
	mTankWarsViewWidget->mVoxeliensSettings->setValue("score", highScores[3].score);
	mTankWarsViewWidget->mVoxeliensSettings->endGroup();

	mTankWarsViewWidget->mVoxeliensSettings->beginGroup("HighScore4");
	mTankWarsViewWidget->mVoxeliensSettings->setValue("name", 4 == mScoreIndex ? mPlayerName : highScores[4].name);
	mTankWarsViewWidget->mVoxeliensSettings->setValue("score", highScores[4].score);
	mTankWarsViewWidget->mVoxeliensSettings->endGroup();

	//Delete 3d text
	delete mTitleText;
	delete mNewHighScoreText;
	delete mEnterYourNameText;
	delete mPressAnyKey;
	for(int ct = 0; ct < NoOfScores; ct++)
	{
		delete mPlayerNameText[ct];
		delete mPlayerScoreText[ct];
	}

	//Syncronise
	mTankWarsViewWidget->mVoxeliensSettings->sync();

	//Fade off the song ready for main menu
	qApp->mMusicPlayer->fadeOffSong();
}

void HighScoreScreen::keyPressEvent(QKeyEvent* event)
{
	GameScreen::keyPressEvent(event);

	if(event->isAutoRepeat())
	{
		return;
	}

	if(mEnteringName)
	{
		//We finish editing on enter or return
		if((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
		{
			mNewHighScoreText->setVisible(false);
			mEnterYourNameText->setVisible(false);
			mTitleText->setVisible(true);
			
			mReadyForPressAnyKey = false;
			QTimer::singleShot(1000, this, SLOT(showPressAnyKey(void)));

			mEnteringName = false;
		}
		//Handle backspace
		else if(event->key() == Qt::Key_Backspace)
		{
			if(mPlayerName.isEmpty() == false)
			{
				mPlayerName.remove(mPlayerName.size() - 1, 1);
				mPlayerNameText[mScoreIndex]->setText(mPlayerName);
			}
		}
		else
		{
			//Letters get appended to the string
			QString text = event->text();
			if(text.isEmpty() == false)
			{
				if(mPlayerName.size() < 8)
				{
					QChar charVal = text.at(0);
					if(charVal.isLetterOrNumber() || charVal.isSpace() || (charVal == QChar('.')))
					{
						charVal = charVal.toUpper();
						char asciiVal = charVal.toAscii();
						mPlayerName += asciiVal;
						mPlayerNameText[mScoreIndex]->setText(mPlayerName);
					}
				}
			}
		}
	}
	else
	{
		//Not entering name. Any key exits the high score table
		if(mReadyForPressAnyKey)
		{
			mTankWarsViewWidget->setScreen(mTankWarsViewWidget->mMainMenuScreen);
		}
	}
}

void HighScoreScreen::mousePressEvent(QMouseEvent* event)
{
	GameScreen::mousePressEvent(event);

	if(mReadyForPressAnyKey)
	{
		mTankWarsViewWidget->setScreen(mTankWarsViewWidget->mMainMenuScreen);
	}
}

void HighScoreScreen::showPressAnyKey(void)
{
	mPressAnyKey->setVisible(true);
	mReadyForPressAnyKey = true;
}
