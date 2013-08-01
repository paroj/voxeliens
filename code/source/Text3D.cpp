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

#include "Text3D.h"

#include "Entity.h"

#include "TankWarsApplication.h"

Text3D::Text3D(QObject * parent)
	:QObject(parent)
{
	mPosition = QVector3D(0,0,0);
}

Text3D::~Text3D()
{
	clearText();
}

void Text3D::setText(const QString& text)
{
	clearText();

	//Set new text.
	for(int ct = 0; ct < text.size(); ct++)
	{
		Thermite::Object* charObject = new Thermite::Object();
		Thermite::Entity* charEntity = new Thermite::Entity("", "MeshMaterial", charObject);
		charObject->setPosition(QVector3D(ct * 7, 0, 0) + mPosition);
		mCharObjects.append(charObject);
		qApp->mObjectList.append(charObject);

		QChar c = text.at(ct);
		if(c == QChar(' '))
		{
			charEntity->setMeshName("");
		}
		else if(c == QChar('.'))
		{
			charObject->setPosition(QVector3D(ct * 7 + 3, 3, 0) + mPosition);
			charEntity->setMeshName("WhiteCube.mesh");
		}
		else if(c == QChar(':'))
		{
			charEntity->setMeshName("Colon.mesh");
		}
		else if(c == QChar('!'))
		{
			charEntity->setMeshName("ExclamationMark.mesh");
		}
		else if(c == QChar('*')) //We use astrix for multiply, so we replace here with a times symbol
		{
			charObject->setPosition(QVector3D(ct * 7, 2, 0) + mPosition); //Don't know why the plus 2 is required...
			charEntity->setMeshName("times.mesh");
		}
		else
		{
			QString meshName = QString("%1.mesh").arg(c.toUpper());
			charEntity->setMeshName(meshName);
		}
	}
}

void Text3D::setVisible(bool visible)
{
	for(int ct = 0; ct < mCharObjects.size(); ct++)
	{
		mCharObjects[ct]->getComponent()->setEnabled(visible);
	}
}

void Text3D::clearText(void)
{
	//Clear existing text.
	for(int ct = 0; ct < mCharObjects.size(); ct++)
	{
		Thermite::Object* charObject = mCharObjects.at(ct);
		qApp->mObjectList.removeOne(charObject);
		delete charObject;
	}

	mCharObjects.clear();
}