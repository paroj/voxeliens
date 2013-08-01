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

#ifndef TERRAINGENERATION_H_
#define TERRAINGENERATION_H_

#include "ThermiteForwardDeclarations.h"

#include <QString>
#include <QColor>

namespace TerrainTypes
{
	enum TerrainType
	{
		Rocky,
		Mountain,
		Desert,		
		NoOfTypes
	};
}
typedef TerrainTypes::TerrainType TerrainType;

void clearMap(Thermite::Volume* volume);
void generateMap(TerrainType terrainType, int terrainSeed, Thermite::Volume* volume);
void generateRockyMap(int terrainSeed, Thermite::Volume* volume);
void generateMountainMap(int terrainSeed, Thermite::Volume* volume);
void generateMarsMap(int terrainSeed, Thermite::Volume* volume);
void generateDesertMap(int terrainSeed, Thermite::Volume* volume);
void generateDesertMap2(int terrainSeed, Thermite::Volume* volume);

bool loadCSV(const QString& filename, Thermite::Volume* volume, qint32 posX, qint32 posY, qint32 posZ);

quint16 QColorToMaterial(const QColor& colour);

#endif //TERRAINGENERATION_H_
