#ifndef SCREEN_H_
#define SCREEN_H_

#include "Object.h"

#include <QObject>

#include "ThermiteForwardDeclarations.h"

class Keyboard;
class Mouse;
class TankWarsViewWidget;

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

class Screen : public QObject
{
	Q_OBJECT
public:
	virtual void initialise(){};
	virtual void preUpdate(){};
	virtual void update(){};
	virtual void postUpdate(){};
	virtual void shutdown(){};

	//We don't derive from QWidget, but we give the event handlers the same names for simplicity.
	virtual void keyPressEvent(QKeyEvent* event){};
	virtual void keyReleaseEvent(QKeyEvent* event){};

	virtual void mousePressEvent(QMouseEvent* event){};
	virtual void mouseReleaseEvent(QMouseEvent* event){};
	virtual void mouseMoveEvent(QMouseEvent* event){};

	virtual void wheelEvent(QWheelEvent* event){};
};

#endif //GAMESCREEN_H_