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

#include "Mouse.h"

Mouse::Mouse(QObject * parent)
	:QObject(parent)
	,mWheelDelta(0)
{
}

bool Mouse::isPressed(int mouseButton)
{
	//Note - I'd rather pass a Qt::MouseButton in a parameter to this 
	//function and avoid the class, but I had problems registering it
	//with qScriptRegisterMetaType().
	Qt::MouseButton mb = static_cast<Qt::MouseButton>(mouseButton);

	return mMouseButtons & mb;
}

void Mouse::press(int mouseButton)
{
	//Note - I'd rather pass a Qt::MouseButton in a parameter to this 
	//function and avoid the class, but I had problems registering it
	//with qScriptRegisterMetaType().
	Qt::MouseButton mb = static_cast<Qt::MouseButton>(mouseButton);

	mMouseButtons |= mb;
}

void Mouse::release(int mouseButton)
{
	//Note - I'd rather pass a Qt::MouseButton in a parameter to this 
	//function and avoid the class, but I had problems registering it
	//with qScriptRegisterMetaType().
	Qt::MouseButton mb = static_cast<Qt::MouseButton>(mouseButton);

	mMouseButtons &= ~mb;
}

void Mouse::releaseAll(void)
{
	mMouseButtons = Qt::NoButton;
}

const QPoint& Mouse::position(void)
{
	return mPosition;
}

void Mouse::setPosition(const QPoint& pos)
{
	mPosition = pos;
}

const QPoint& Mouse::previousPosition(void)
{
	return mPreviousPosition;
}

void Mouse::setPreviousPosition(const QPoint& pos)
{
	mPreviousPosition = pos;
}

void Mouse::modifyWheelDelta(int wheelDelta)
{
	mWheelDelta += wheelDelta;
}

int Mouse::getWheelDelta(void)
{
	return mWheelDelta;
}

void Mouse::resetWheelDelta(void)
{
	mWheelDelta = 0;
}
