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

#include "TerrainGeneration.h"

#include "Perlin.h"

//Thermite
#include "Volume.h"

//PolyVox
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"

#include "PolyVoxUtil/VolumeChangeTracker.h"

#include <QColor>

using namespace Thermite;
using namespace PolyVox;

template< template<typename> class VolumeType, typename VoxelType>
	Vector3DFloat computeSobelGradientCopy(const typename VolumeType<VoxelType>::Sampler& volIter)
	{
		static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
			{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

			const int pVoxel1nx1ny1nz = volIter.peekVoxel1nx1ny1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx1ny0pz = volIter.peekVoxel1nx1ny0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx1ny1pz = volIter.peekVoxel1nx1ny1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx0py1nz = volIter.peekVoxel1nx0py1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx0py0pz = volIter.peekVoxel1nx0py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx0py1pz = volIter.peekVoxel1nx0py1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx1py1nz = volIter.peekVoxel1nx1py1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx1py0pz = volIter.peekVoxel1nx1py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1nx1py1pz = volIter.peekVoxel1nx1py1pz().getMaterial() > 0 ? 1: 0;

			const int pVoxel0px1ny1nz = volIter.peekVoxel0px1ny1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px1ny0pz = volIter.peekVoxel0px1ny0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px1ny1pz = volIter.peekVoxel0px1ny1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px0py1nz = volIter.peekVoxel0px0py1nz().getMaterial() > 0 ? 1: 0;
			//const int pVoxel0px0py0pz = volIter.peekVoxel0px0py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px0py1pz = volIter.peekVoxel0px0py1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px1py1nz = volIter.peekVoxel0px1py1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px1py0pz = volIter.peekVoxel0px1py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel0px1py1pz = volIter.peekVoxel0px1py1pz().getMaterial() > 0 ? 1: 0;

			const int pVoxel1px1ny1nz = volIter.peekVoxel1px1ny1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px1ny0pz = volIter.peekVoxel1px1ny0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px1ny1pz = volIter.peekVoxel1px1ny1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px0py1nz = volIter.peekVoxel1px0py1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px0py0pz = volIter.peekVoxel1px0py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px0py1pz = volIter.peekVoxel1px0py1pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px1py1nz = volIter.peekVoxel1px1py1nz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px1py0pz = volIter.peekVoxel1px1py0pz().getMaterial() > 0 ? 1: 0;
			const int pVoxel1px1py1pz = volIter.peekVoxel1px1py1pz().getMaterial() > 0 ? 1: 0;

			const int xGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
				weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
				pVoxel1nx1ny1pz - weights[0][1][0] * pVoxel1nx0py1nz -
				weights[1][1][0] * pVoxel1nx0py0pz - weights[2][1][0] *
				pVoxel1nx0py1pz - weights[0][2][0] * pVoxel1nx1py1nz -
				weights[1][2][0] * pVoxel1nx1py0pz - weights[2][2][0] *
				pVoxel1nx1py1pz + weights[0][0][2] * pVoxel1px1ny1nz +
				weights[1][0][2] * pVoxel1px1ny0pz + weights[2][0][2] *
				pVoxel1px1ny1pz + weights[0][1][2] * pVoxel1px0py1nz +
				weights[1][1][2] * pVoxel1px0py0pz + weights[2][1][2] *
				pVoxel1px0py1pz + weights[0][2][2] * pVoxel1px1py1nz +
				weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
				pVoxel1px1py1pz);

			const int yGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
				weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
				pVoxel1nx1ny1pz + weights[0][2][0] * pVoxel1nx1py1nz +
				weights[1][2][0] * pVoxel1nx1py0pz + weights[2][2][0] *
				pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz -
				weights[1][0][1] * pVoxel0px1ny0pz - weights[2][0][1] *
				pVoxel0px1ny1pz + weights[0][2][1] * pVoxel0px1py1nz +
				weights[1][2][1] * pVoxel0px1py0pz + weights[2][2][1] *
				pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz -
				weights[1][0][2] * pVoxel1px1ny0pz - weights[2][0][2] *
				pVoxel1px1ny1pz + weights[0][2][2] * pVoxel1px1py1nz +
				weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
				pVoxel1px1py1pz);

			const int zGrad(- weights[0][0][0] * pVoxel1nx1ny1nz +
				weights[2][0][0] * pVoxel1nx1ny1pz - weights[0][1][0] *
				pVoxel1nx0py1nz + weights[2][1][0] * pVoxel1nx0py1pz -
				weights[0][2][0] * pVoxel1nx1py1nz + weights[2][2][0] *
				pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz +
				weights[2][0][1] * pVoxel0px1ny1pz - weights[0][1][1] *
				pVoxel0px0py1nz + weights[2][1][1] * pVoxel0px0py1pz -
				weights[0][2][1] * pVoxel0px1py1nz + weights[2][2][1] *
				pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz +
				weights[2][0][2] * pVoxel1px1ny1pz - weights[0][1][2] *
				pVoxel1px0py1nz + weights[2][1][2] * pVoxel1px0py1pz -
				weights[0][2][2] * pVoxel1px1py1nz + weights[2][2][2] *
				pVoxel1px1py1pz);

			//Note: The above actually give gradients going from low density to high density.
			//For our normals we want the the other way around, so we switch the components as we return them.
			return Vector3DFloat(static_cast<float>(-xGrad),static_cast<float>(-yGrad),static_cast<float>(-zGrad));
	}

quint16 QColorToMaterial(const QColor& colour)
{
	//Map an intensity of 255 (1.0f) to our largest value and 0 to 0.0f
	uint8_t r = colour.redF() * 15.0f + 0.5f; //0.5 for rounding
	uint8_t g = colour.greenF() * 15.0f + 0.5f; //0.5 for rounding
	uint8_t b = colour.blueF() * 15.0f + 0.5f; //0.5 for rounding
	uint8_t a = colour.alphaF() * 15.0f + 0.5f; //0.5 for rounding

	uint16_t result = (r << 12) | (g << 8) | (b << 4) | (a);
	return result;
}

void generateMap(TerrainType terrainType, int terrainSeed, Thermite::Volume* volume)
{
	//First clear the map
	clearMap(volume);

	//Now generate the level
	if(terrainType == TerrainTypes::Rocky)
	{
		generateRockyMap(terrainSeed, volume);
	}
	else if(terrainType == TerrainTypes::Mountain)
	{
		generateMountainMap(terrainSeed, volume);
	}
	else if(terrainType == TerrainTypes::Desert)
	{
		generateDesertMap2(terrainSeed, volume);
	}
	else
	{
		generateDesertMap2(terrainSeed, volume);
	}

	//volume->writeToFile("./volumes/level8.vol");

	//volume->readFromFile("./volumes/level8.vol");

	//volume->computeAmbientOcclusion();
}

void clearMap(Thermite::Volume* volume)
{
	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	for(int z = 0; z < pPolyVoxVolume->getDepth(); z++)
	{
		for(int y = 0; y < pPolyVoxVolume->getHeight(); y++)
		{
			for(int x = 0; x < pPolyVoxVolume->getWidth(); x++) 
			{
				Material16 voxel;
				voxel.setMaterial(0);
				pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
			}
		}
	}
}

void generateRockyMap(int terrainSeed, Thermite::Volume* volume)
{
	const int mapWidth = 128;
	const int mapHeight = 24;
	const int mapDepth = 128;

	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	int octaves = 2;
	float freq = 0.02f;
	float amp = 1.0f;

	float yScaleFactor = 2.0f; //Bigger values cause more overhangs
	float threshold = 0.15f; //Positive give more space
	float waterThreshold = 0.0f; //Positive gives more water

	//Dodgy selection of water threshold based on map...
	if(terrainSeed == 86785) //Level 0
	{
		waterThreshold = -0.24f;
	}
	if(terrainSeed == 3256) //Level 1
	{
		waterThreshold = -0.25f;
	}
	if(terrainSeed == 3868) //Level 2
	{
		waterThreshold = -0.18f;
	}

	Perlin perlin(octaves, freq, amp, terrainSeed);

	quint16 rockMaterial = QColorToMaterial(QColor(133, 98, 70));
	quint16 grassMaterial = QColorToMaterial(QColor(60, 152, 43));
	quint16 waterMaterial = QColorToMaterial(Qt::blue);

	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			for(int x = 0; x < mapWidth; x++) 
			{							
				float perlinVal = perlin.Get3D(x, y, z);

				//Use height and depth to add voxels near top and fade off voxels near ground.
				float height = y / static_cast<float>(mapHeight-1);
				float depth = 1.0f - height;

				height *= height;
				height *= height;
				height *= height;

				depth *= depth;
				depth *= depth;
				depth *= depth;
				depth *= depth;

				perlinVal = perlinVal /*+ depth*/ - height;

				Material16 voxel;
				if(perlinVal > threshold)
				{
					voxel.setMaterial(rockMaterial);
				}
				else
				{
					voxel.setMaterial(0);
				}

				if((y < 3) && (perlinVal > waterThreshold))
				{
					voxel.setMaterial(rockMaterial);
				}

				if((y < 1))
				{
					voxel.setMaterial(rockMaterial);
				}

				pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
			}
		}
	}

	//Hack to make this particular map easier
	if(terrainSeed == 86785)
	{
		for(int z = 64; z < mapDepth; z++)
		{
			for(int y = 3; y < mapHeight; y++)
			{
				for(int x = 0; x < mapWidth; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}
	}

	//Hack to make this particular map easier
	if(terrainSeed == 3868)
	{
		for(int z = 40; z < 50; z++)
		{
			for(int y = 3; y < 5; y++)
			{
				for(int x = 103; x < 113; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}

		for(int z = 60; z < 85; z++)
		{
			for(int y = 3; y < 6; y++)
			{
				for(int x = 60; x < 85; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}
	}	

	PolyVox::RawVolume<PolyVox::Material16> volCopy(pPolyVoxVolume->getEnclosingRegion());
	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = 0; y < mapHeight; y++)
		{
			for(int x = 0; x < mapWidth; x++) 
			{
				Material16 temp = pPolyVoxVolume->getVoxelAt(x,y,z);
				volCopy.setVoxelAt(x,y,z,temp);
			}
		}
	}

	PolyVox::RawVolume<PolyVox::Material16>::Sampler sampler(&volCopy);

	//Go through and identify the top voxels.
	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = mapHeight-2; y > 0; y--) //Top down
		{
			for(int x = 0; x < mapWidth; x++) 
			{
				sampler.setPosition(x,y,z);

				Material16 voxelAbove = volCopy.getVoxelAt(x,y+1,z);
				Material16 voxelCurrent = volCopy.getVoxelAt(x,y,z);
				Material16 voxelBelow = volCopy.getVoxelAt(x,y-1,z);

				Vector3DFloat normal = computeSobelGradientCopy<PolyVox::RawVolume, PolyVox::Material16>(sampler);
				normal.normalise();

				//For grass voxels on hills we checks the normal
				bool grassVoxel = ((voxelAbove.getMaterial() == 0) && 
					(voxelCurrent.getMaterial() == 0) && 
					(voxelBelow.getMaterial() == rockMaterial) &&
					normal.getY() > 0.71) &&
					(y > 3);

				//For those at ground level we don't check the normal.
				grassVoxel |= ((voxelAbove.getMaterial() == 0) &&
					(voxelBelow.getMaterial() == rockMaterial)) &&
					(y == 3);

				if(grassVoxel) //Top voxel
				{
					Material16 voxel(grassMaterial);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);

					//If it's a top voxel above an altitude, then some random chance of adding a tree.
					if(y > 10)
					{
						if(qrand() % 500 == 0)
						{
							loadCSV("./voxels/OakTree5.csv", volume, x - 13, y, z - 13);
						}
						if(qrand() % 500 == 1)
						{
							loadCSV("./voxels/OakTree6.csv", volume, x - 13, y, z - 13);
						}
						if(qrand() % 500 == 2)
						{
							loadCSV("./voxels/OakTree7.csv", volume, x - 13, y, z - 13);
						}
						if(qrand() % 500 == 3)
						{
							loadCSV("./voxels/OakTree8.csv", volume, x - 13, y, z - 13);
						}
						if(qrand() % 500 == 4)
						{
							loadCSV("./voxels/OakTree9.csv", volume, x - 13, y, z - 13);
						}
					}
				}
			}
		}
	}

	//Fill in the water
	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = 1; y <=2; y++) //Top down
		{
			for(int x = 0; x < mapWidth; x++) 
			{
				sampler.setPosition(x,y,z);
				Material16 voxelCurrent = volCopy.getVoxelAt(x,y,z);

				if(voxelCurrent.getMaterial() == 0)
				{
					Material16 voxel(waterMaterial);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}
	}

	return;
}

void generateMountainMap(int terrainSeed, Thermite::Volume* volume)
{
	const int mapWidth = 128;
	const int mapHeight = 31;
	const int mapDepth = 128;

	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	int octaves = 4;
	float freq = 0.02f;
	float amp = 1.0f;

	float lakeThreshold = -0.3f; //More positive values give more lake

	Perlin perlin(octaves, freq, amp, terrainSeed);

	quint16 rockMaterial = QColorToMaterial(QColor(96, 96, 96));
	quint16 frozenLakeMaterial = QColorToMaterial(QColor(192, 192, 255));
	quint16 snowMaterial = QColorToMaterial(Qt::white);

	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = 0; y < mapHeight; y++)
		{			
			for(int x = 0; x < mapWidth; x++) 
			{			
				float perlinVal = perlin.Get(x, z);

				Material16 voxel;

				if(perlinVal > lakeThreshold)
				{
					//Not lakes
					perlinVal *= mapHeight;
					perlinVal *= 1.2f;
					
					if(y < perlinVal)
					{
						voxel.setMaterial(rockMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}

					if(y < 4)
					{
						voxel.setMaterial(rockMaterial);
					}
				}
				else
				{
					//Lakes
					if(y > 2)
					{
						voxel.setMaterial(0);
					}
					else if(y > 0)
					{
						voxel.setMaterial(frozenLakeMaterial);
					}
					else
					{
						voxel.setMaterial(rockMaterial);
					}
				}

				pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
			}
		}
	}

	//Hack to make this particular map easier
	if(terrainSeed == 6785)
	{
		for(int z = 0; z < 127; z++)
		{
			for(int y = 4; y < 10; y++)
			{
				for(int x = 0; x < 5; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}

		for(int z = 100; z < 127; z++)
		{
			for(int y = 4; y < 10; y++)
			{
				for(int x = 0; x < 30; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}

		for(int z = 80; z < 85; z++)
		{
			for(int y = 4; y < 10; y++)
			{
				for(int x = 0; x < 30; x++) 
				{
					Material16 voxel(0);
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
		}
	}

	qsrand(101);

	//Go through and identify the top voxels.
	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = mapHeight-2; y > 0; y--) //Top down
		{
			for(int x = 0; x < mapWidth; x++) 
			{
				Material16 voxelAbove = pPolyVoxVolume->getVoxelAt(x,y+1,z);
				Material16 voxelCurrent = pPolyVoxVolume->getVoxelAt(x,y,z);

				if((voxelAbove.getMaterial() == 0) && (voxelCurrent.getMaterial() == rockMaterial)) //Top voxel
				{
					if(y == 3)
					{
						Material16 voxel(snowMaterial);
						pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
					}
					else
					{
						int iColour = (y * 32) - 64;
						iColour = qMin(255, iColour);

						QColor mountainSideColour(iColour, iColour, iColour);
						Material16 voxel(QColorToMaterial(mountainSideColour));
						pPolyVoxVolume->setVoxelAt(x,y,z,voxel);					
					}

					if((qrand() % 75 == 0) && (y > 5) && (y < 20))
					{
						loadCSV("./voxels/PineTree2.csv", volume, x - 13, y, z - 13);
					}
				}
			}
		}
	}

	return;
}

void generateMarsMap(int terrainSeed, Thermite::Volume* volume)
{
	const int mapWidth = 128;
	const int mapHeight = 16;
	const int mapDepth = 128;

	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	//Create a grid of Perlin noise values
	Perlin perlin(2,4,1,234);
	float perlinValues[mapWidth][mapDepth];
	float minPerlinValue = 1000.0f;
	float maxPerlinValue = -1000.0f;
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{
			perlinValues[x][z] = perlin.Get(x /static_cast<float>(mapWidth-1), z / static_cast<float>(mapDepth-1));
			minPerlinValue = std::min(minPerlinValue, perlinValues[x][z]);
			maxPerlinValue = std::max(maxPerlinValue, perlinValues[x][z]);
		}
	}

	//Normalise values so that th smallest is 0.0 and the biggest is 1.0
	float range = maxPerlinValue - minPerlinValue;
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{
			perlinValues[x][z] = (perlinValues[x][z] - minPerlinValue) / range;
		}
	}

	uint16_t rockMaterial = QColorToMaterial(Qt::darkGray);
	uint16_t surfaceMaterial = QColorToMaterial(Qt::darkRed);
	uint16_t waterMaterial = QColorToMaterial(Qt::darkBlue);

	//Copy the data into the volume
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{										
			
			Material16 voxel;
			//Water
			if(perlinValues[x][z] < 0.1)
			{
				for(int y = 0; y < mapHeight; y++)
				{			
					if(y == 0)
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y < 3)
					{
						voxel.setMaterial(waterMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
			//Main ground
			else if(perlinValues[x][z] < 0.8)
			{
				for(int y = 0; y < mapHeight; y++)
				{		
					if(y < 3)
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y == 3)
					{
						voxel.setMaterial(surfaceMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
			//Cliffs
			else
			{
				int terrainHeight = perlinValues[x][z] * (mapHeight-1);
				for(int y = 0; y < mapHeight; y++)
				{		
					if(y < terrainHeight)
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y == terrainHeight)
					{
						voxel.setMaterial(surfaceMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}			
		}
	}

	//loadCSV("./voxels/SatalliteDish.csv", volume, 50, 4, 100);
	//loadCSV("./voxels/LandingPad.csv", volume, 80, 4, 90);
	loadCSV("./voxels/UmbrellaTower.csv", volume, 50, 4, 100);

	return;
}

void generateDesertMap(int terrainSeed, Thermite::Volume* volume)
{
	const int mapWidth = 128;
	const int mapHeight = 16;
	const int mapDepth = 128;

	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	//Create a grid of Perlin noise values
	Perlin perlin(2,4,1,87);
	float perlinValues[mapWidth][mapDepth];
	float minPerlinValue = 1000.0f;
	float maxPerlinValue = -1000.0f;
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{
			perlinValues[x][z] = perlin.Get(x /static_cast<float>(mapWidth-1), z / static_cast<float>(mapDepth-1));
			minPerlinValue = std::min(minPerlinValue, perlinValues[x][z]);
			maxPerlinValue = std::max(maxPerlinValue, perlinValues[x][z]);
		}
	}

	//Normalise values so that th smallest is 0.0 and the biggest is 1.0
	float range = maxPerlinValue - minPerlinValue;
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{
			perlinValues[x][z] = (perlinValues[x][z] - minPerlinValue) / range;
		}
	}

	uint16_t rockMaterial = QColorToMaterial(Qt::darkGray);
	uint16_t surfaceMaterial = QColorToMaterial(Qt::yellow);
	uint16_t waterMaterial = QColorToMaterial(Qt::darkBlue);

	//Copy the data into the volume
	for(int z = 0; z < mapDepth; z++)
	{
		for(int x = 0; x < mapWidth; x++) 
		{										
			
			Material16 voxel;
			//Water
			if(perlinValues[x][z] < 0.1)
			{
				for(int y = 0; y < mapHeight; y++)
				{			
					if(y == 0)
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y < 3)
					{
						voxel.setMaterial(waterMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
			//Main ground
			else if(perlinValues[x][z] < 0.7)
			{
				for(int y = 0; y < mapHeight; y++)
				{		
					if(y < 3)
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y == 3)
					{
						voxel.setMaterial(surfaceMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}
			//Cliffs
			else
			{
				int terrainHeight = perlinValues[x][z] * (mapHeight-1);
				for(int y = 0; y < mapHeight; y++)
				{		
					int offset = 6; //This code copied from mars version - offset gets rid of cliffs.
					if(y < (terrainHeight - offset))
					{
						voxel.setMaterial(rockMaterial);
					}
					else if(y == (terrainHeight - offset))
					{
						voxel.setMaterial(rockMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}	
					pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
				}
			}			
		}
	}

	loadCSV("./voxels/Cactus1.csv", volume, 40, 4, 80);
	loadCSV("./voxels/Cactus2.csv", volume, 25, 4, 25);
	loadCSV("./voxels/Cactus1.csv", volume, 30, 4, 110);
	loadCSV("./voxels/Cactus2.csv", volume, 105, 4, 110);

	return;
}

//Based on code from Sproxel
bool loadCSV(const QString& filename, Thermite::Volume* volume, qint32 posX, qint32 posY, qint32 posZ)
{
	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	int fscanfStatus = 0;
	FILE* fp = fopen(filename.toAscii().constData(), "rb");
	if (!fp) return false;

	// Read the dimensions
	int sizeX = 0, sizeY = 0, sizeZ = 0;
	fscanfStatus = fscanf(fp, "%d,%d,%d\n", &sizeX, &sizeY, &sizeZ);

	// Read the data
	QColor color;
	for (int y = sizeY-1; y >= 0; y--)
	{
		for (int z = 0; z < sizeZ; z++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				int ir, ig, ib, ia;
				fscanfStatus = fscanf(fp, "#%02X%02X%02X%02X,", &ir, &ig, &ib, &ia);

				/*float r = ir / (float)0xff;
				float g = ig / (float)0xff;
				float b = ib / (float)0xff;
				float a = ia / (float)0xff;*/

				QColor color(ir, ig, ib, ia);

				if(ia > 0)
				{
					PolyVox::Material16 voxel;
					voxel.setMaterial(QColorToMaterial(color));

					int xToWrite = posX + x;
					int yToWrite = posY + y;
					int zToWrite = posZ + z;
					if((xToWrite >= 0) && (xToWrite < pPolyVoxVolume->getWidth()) &&
						(yToWrite >= 0) && (yToWrite < pPolyVoxVolume->getHeight()) &&
						(zToWrite >= 0) && (zToWrite < pPolyVoxVolume->getDepth()))
					{
						pPolyVoxVolume->setVoxelAt(xToWrite, yToWrite, zToWrite, voxel);
					}
				}

				if (x != sizeZ-1)
					fscanfStatus = fscanf(fp, ",");
			}
			fscanfStatus = fscanf(fp, "\n");
		}
		fscanfStatus = fscanf(fp, "\n");
	}

	fclose(fp);
	return true;
}

void generateDesertMap2(int terrainSeed, Thermite::Volume* volume)
{
	const int mapWidth = 128;
	const int mapHeight = 31;
	const int mapDepth = 128;

	PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume = volume->m_pPolyVoxVolume;

	int octaves = 1;
	float freq = 0.02f;
	float amp = 1.0f;

	float lakeThreshold = -0.3f; //More positive values give more lake

	Perlin perlin(octaves, freq, amp, terrainSeed);

	quint16 rockMaterial = QColorToMaterial(QColor(96, 96, 96));
	quint16 lakeMaterial = QColorToMaterial(QColor(64, 64, 255));
	quint16 sandMaterial = QColorToMaterial(Qt::yellow);

	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = 0; y < mapHeight; y++)
		{			
			for(int x = 0; x < mapWidth; x++) 
			{			
				float perlinVal = perlin.Get(x, z);

				Material16 voxel;

				if(perlinVal > lakeThreshold)
				{
					//Not lakes
					perlinVal *= mapHeight;

					float heightMultiplier; //Affects height of hills
					heightMultiplier = (terrainSeed == 87987 ? 0.8f : 0.9f);
					perlinVal *= heightMultiplier; 
					
					if(y < perlinVal)
					{
						voxel.setMaterial(rockMaterial);
					}
					else
					{
						voxel.setMaterial(0);
					}

					if(y < 4)
					{
						voxel.setMaterial(rockMaterial);
					}
				}
				else
				{
					//Lakes
					if(y > 2)
					{
						voxel.setMaterial(0);
					}
					else if(y > 0)
					{
						voxel.setMaterial(lakeMaterial);
					}
					else
					{
						voxel.setMaterial(rockMaterial);
					}
				}

				pPolyVoxVolume->setVoxelAt(x,y,z,voxel);
			}
		}
	}

	qsrand(100);

	//Go through and identify the top voxels.
	for(int z = 0; z < mapDepth; z++)
	{
		for(int y = mapHeight-2; y > 0; y--) //Top down
		{
			for(int x = 0; x < mapWidth; x++) 
			{
				Material16 voxelAbove = pPolyVoxVolume->getVoxelAt(x,y+1,z);
				Material16 voxelCurrent = pPolyVoxVolume->getVoxelAt(x,y,z);

				if((voxelAbove.getMaterial() == 0) && (voxelCurrent.getMaterial() == rockMaterial)) //Top voxel
				{
					if(y == 3) //Sand
					{
						Material16 voxel(sandMaterial);
						pPolyVoxVolume->setVoxelAt(x,y,z,voxel);

						//if((qrand() % 2001 == 0)) //for first two desert levels
						if((qrand() % 2502 == 0)) //for last desert level
						{
							loadCSV("./voxels/Cactus1.csv", volume, x - 13, 3, z - 2);
						}
					}
					else if(y > 4)//On a hill
					{
						//if((qrand() % 290 == 0)) //for first two desert levels
						if((qrand() % 246 == 0)) //for last desert level
						{
							loadCSV("./voxels/PalmTree2.csv", volume, x - 4, y, z - 4);
						}
					}
				}
			}
		}
	}

	return;
}