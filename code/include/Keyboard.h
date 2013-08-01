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

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <QHash>

class Keyboard : public QObject
{
	Q_OBJECT

public:
	Keyboard(QObject * parent = 0);

public slots:
	bool isPressed(int key);
	void press(int key);
	void release(int key);
	void releaseAll(void);

private:
	// Do we really need to keep track of a pair here? I.e. the key and whether it is pressed?
	// A list of keys which are pressed might be enough. To check whether it is pressed we have to
	// search for it in the list anyway, so might as well just return baed on whether it is found or not.
	QHash<int, bool> mKeyStates;
};	

#endif //KEYBOARD_H_