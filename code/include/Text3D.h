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

#ifndef TEXT3D_H_
#define TEXT3D_H_

#include <QObject>

#include "Object.h"

class Text3D : public QObject
{
	Q_OBJECT

public:
	Text3D(QObject * parent = 0);
	~Text3D();

	void setText(const QString& text);
	void setPosition(const QVector3D& position) { mPosition = position; }

	void setVisible(bool visible);

	void clearText(void);

public:
	QList<Thermite::Object*> mCharObjects;
	QVector3D mPosition;
};

#endif //TEXT3D_H_