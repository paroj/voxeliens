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

#ifndef GAMESCREEN_H_
#define GAMESCREEN_H_

#include "Object.h"

#include "Screen.h"

#include "ThermiteForwardDeclarations.h"

class Keyboard;
class Mouse;
class TankWarsViewWidget;

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class GameScreen : public Screen
{
	Q_OBJECT
public:
	GameScreen(TankWarsViewWidget* tankWarsViewWidget);

	void initialise();
	void preUpdate();
	void update();
	void postUpdate();
	void shutdown();

	//We don't derive from QWidget, but we give the event handlers the same names for simplicity.
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

	void wheelEvent(QWheelEvent* event);

public:
	TankWarsViewWidget* mTankWarsViewWidget;

	//Input
	Keyboard* keyboard;
	Mouse* mouse;

	//Bunch of static stuff here needs rethinking. We only want one copy
	//of these, but we want them available in all the derived classes.
	//Camera
	static Thermite::Object* cameraNode;
	static QVector3D cameraFocusPoint;
	static float cameraElevationAngle;
	static float cameraRotationAngle;
	static float cameraDistance;

	//Times
	static float currentTimeInSeconds;
	static float timeElapsedInSeconds;
	static float previousTimeInSeconds;

	//Lighting
	static Thermite::Object* lightObject;
	static Thermite::Light* light0;

	//Skybox
	Thermite::Object* skyboxObject;
	Thermite::SkyBox* skyBox;

	//To pause before showing 'press any key'
	bool mReadyForPressAnyKey;
};

#endif //GAMESCREEN_H_