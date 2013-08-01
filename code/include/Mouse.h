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

#ifndef MOUSE_H_
#define MOUSE_H_

#include <QObject>
#include <QPoint>

class Mouse : public QObject
{
	Q_OBJECT

public:
	Mouse(QObject * parent = 0);

	Q_PROPERTY(QPoint position READ position)
	Q_PROPERTY(QPoint previousPosition READ previousPosition)

public slots:
	bool isPressed(int mouseButton);
	void press(int mouseButton);
	void release(int mouseButton);
	void releaseAll(void);

	const QPoint& position(void);
	void setPosition(const QPoint& pos);

	const QPoint& previousPosition(void);
	void setPreviousPosition(const QPoint& pos);

	void modifyWheelDelta(int wheelDelta);
	int getWheelDelta(void);
	void resetWheelDelta(void);

	int positionX(void) {return mPosition.x();}
	int positionY(void) {return mPosition.y();}
	int previousPositionX(void) {return mPreviousPosition.x();}
	int previousPositionY(void) {return mPreviousPosition.y();}

private:
	Qt::MouseButtons mMouseButtons;
	QPoint mPosition;
	QPoint mPreviousPosition;
	int mWheelDelta;
};

#endif //MOUSE_H_