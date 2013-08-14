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

#ifndef MAINMENUSCREEN_H_
#define MAINMENUSCREEN_H_

#include "GameScreen.h"

#include "Entity.h"

#include "Text3D.h"

#include <QTimer>

#include "OgreAxisAlignedBox.h"

#include <SDL.h>
#include <SDL_mixer.h>

class MainMenuScreen : public GameScreen
{
	Q_OBJECT

public:
	MainMenuScreen(TankWarsViewWidget* tankWarsViewWidget);

	virtual void initialise();
	virtual void update();
	virtual void shutdown();

	void moveUp(void);
	void moveDown(void);

	void keyPressEvent(QKeyEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);

	void currentTextUpdated(void);
	void currentTextSelected(void);

public slots:
	void toggleVisibility(void);
	void startPlayingSong(void);

private:
	Text3D* mNewGameText;
	Text3D* mSettingsText;
	Text3D* mBuyFullGameText;
	Text3D* mQuitGameText;

	Text3D* mCurrentText;

	//Bounding boxes
	Ogre::AxisAlignedBox mNewGameTextAABB;
	Ogre::AxisAlignedBox mSettingsTextAABB;
	Ogre::AxisAlignedBox mBuyFullGameTextAABB;
	Ogre::AxisAlignedBox mQuitGameTextAABB;

	QTimer* mFlashingTimer;
	bool mCurrentTextVisible;

	bool mFirstShowing;

	//Sounds
	Mix_Chunk* mThudSound;
	Mix_Chunk* mClickSound;
};

#endif //MAINMENUSCREEN_H_
