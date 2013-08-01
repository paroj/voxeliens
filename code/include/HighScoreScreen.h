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

#ifndef HIGHSCORESCREEN_H_
#define HIGHSCORESCREEN_H_

#include "GameScreen.h"

#include "Entity.h"

#include "Text3D.h"

#include <QTimer>

#include "OgreAxisAlignedBox.h"

struct HighScore
{
	QString name;
	quint32 score;
};

class HighScoreScreen : public GameScreen
{
	Q_OBJECT

	static const int NoOfScores = 5; //DO NOT CHANGE! This is also hardcoded into the settings file code.

public:
	HighScoreScreen(TankWarsViewWidget* tankWarsViewWidget);

	virtual void initialise();
	virtual void update();
	virtual void shutdown();

	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);

public slots:
	void showPressAnyKey(void);

private:
	Text3D* mTitleText;
	Text3D* mNewHighScoreText;
	Text3D* mEnterYourNameText;
	Text3D* mPressAnyKey;
	Text3D* mPlayerNameText[NoOfScores];
	Text3D* mPlayerScoreText[NoOfScores];

	QList<HighScore> highScores;
	int mScoreIndex;
	QString mPlayerName;

	bool mEnteringName;
};

#endif //HIGHSCORESCREEN_H_