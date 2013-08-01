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

#ifndef THERMITE_TANKWARSVIEWWIDGET_H_
#define THERMITE_TANKWARSVIEWWIDGET_H_

#include "OgreWidget.h"

class QWidget;

#include "PolyVoxCore/Vector.h"

#include "PausedScreen.h"
#include "PlayScreen.h"
#include "HighScoreScreen.h"
#include "MainMenuScreen.h"
#include "SettingsDialog.h"

#include <QSettings>

class TankWarsViewWidget : public Thermite::OgreWidget
{
	Q_OBJECT

public:
	TankWarsViewWidget(QWidget* parent=0, Qt::WindowFlags f=0);
	~TankWarsViewWidget();

	void initialise(void);

	void closeEvent(QCloseEvent* event);

public:
	//Screen management
	HighScoreScreen* mHighScoreScreen;
	MainMenuScreen* mMainMenuScreen;
	PausedScreen* mPausedScreen;
	PlayScreen* mPlayScreen;

	QSettings* mVoxeliensSettings;
	::SettingsDialog* mSettingsDialog;
};

#endif /*THERMITE_TANKWARSVIEWWIDGET_H_*/
