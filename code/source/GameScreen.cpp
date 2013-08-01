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

#include "GameScreen.h"

#include <QKeyEvent>
#include <QMouseEvent>

#include "Keyboard.h"
#include "Mouse.h"
#include "TankWarsApplication.h"
#include "TankWarsViewWidget.h"

#include "Light.h"
#include "SkyBox.h"

Thermite::Object* GameScreen::cameraNode = 0;
QVector3D GameScreen::cameraFocusPoint;
float GameScreen::cameraElevationAngle;
float GameScreen::cameraRotationAngle;
float GameScreen::cameraDistance;
Thermite::Object* GameScreen::lightObject = 0;
Thermite::Light* GameScreen::light0 = 0;

float GameScreen::currentTimeInSeconds;
float GameScreen::timeElapsedInSeconds;
float GameScreen::previousTimeInSeconds;

GameScreen::GameScreen(TankWarsViewWidget* tankWarsViewWidget)
	:mTankWarsViewWidget(tankWarsViewWidget)
{
	currentTimeInSeconds = 0.0f;
	timeElapsedInSeconds = 0.0f;
	previousTimeInSeconds = 0.0f;

	keyboard = new Keyboard(0);
	mouse = new Mouse(0);

	//Light setup
	if(!lightObject)
	{
		lightObject = new Object();
		lightObject->setPosition(QVector3D(64,128,64));
		lightObject->lookAt(QVector3D(64,0,64));
		qApp->mObjectList.append(lightObject);
	}
	
	if(!light0)
	{
		light0 = new Thermite::Light(lightObject);
		light0->setType(Thermite::Light::DirectionalLight);
		light0->setColour(QColor(255,255,255));
	}

	//Skybox setup
	skyboxObject = new Object();
	skyBox = new SkyBox(skyboxObject);
	skyBox->setMaterialName("SpaceScapeBluePurpleMaterial");
	qApp->mObjectList.append(skyboxObject);
}

void GameScreen::initialise()
{
	//Camera setup
	if(cameraNode == 0)
	{
		cameraNode = new Object();
		cameraFocusPoint = QVector3D(0, 0, 0);
		cameraElevationAngle = 30.0;
		cameraRotationAngle = 0.0;
		cameraDistance = 145.0;
		mTankWarsViewWidget->mCamera->setParent(cameraNode);
		qApp->mObjectList.append(cameraNode);
	}
}

void GameScreen::preUpdate()
{
	previousTimeInSeconds = currentTimeInSeconds;
	currentTimeInSeconds = Thermite::globals.timeSinceAppStart() * 0.001f;
	timeElapsedInSeconds = currentTimeInSeconds - previousTimeInSeconds;
}

void GameScreen::update()
{
}

void GameScreen::postUpdate()
{
	mouse->setPreviousPosition(mouse->position());
	mouse->resetWheelDelta();
}

void GameScreen::shutdown()
{
}

void GameScreen::keyPressEvent(QKeyEvent* event)
{
	if(event->isAutoRepeat())
	{
		return;
	}

	keyboard->press(event->key());
}

void GameScreen::keyReleaseEvent(QKeyEvent* event)
{
	if(event->isAutoRepeat())
	{
		return;
	}

	keyboard->release(event->key());
}

void GameScreen::mousePressEvent(QMouseEvent* event)
{
	mouse->press(event->button());
	
	//Update the mouse position as well or we get 'jumps'
	mouse->setPosition(event->pos());
	mouse->setPreviousPosition(mouse->position());
}

void GameScreen::mouseReleaseEvent(QMouseEvent* event)
{
	mouse->release(event->button());
}

void GameScreen::mouseMoveEvent(QMouseEvent* event)
{
	mouse->setPosition(event->pos());
}

void GameScreen::wheelEvent(QWheelEvent* event)
{
	mouse->modifyWheelDelta(event->delta());
}
