/*******************************************************************************
Copyright (c) 2005-2009 David Williams

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

#include "AmbientOcclusionTask.h"

#include "PolyVoxCore/Density.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/Material.h"

#include "PolyVoxCore/AmbientOcclusionCalculator.h"

#include <QDebug>
#include <QMutex>
#include <QTime>

using namespace PolyVox;

namespace Thermite
{
	PolyVox::RawVolume<PolyVox::Density8>* AmbientOcclusionTask::mThresholdVolume = 0;
	PolyVox::RawVolume<PolyVox::Density8>* AmbientOcclusionTask::mBlurredVolume = 0;

	AmbientOcclusionTask::AmbientOcclusionTask(PolyVox::SimpleVolume<PolyVox::Material16>* volume, PolyVox::Array<3, uint8_t>* ambientOcclusionVolume, PolyVox::Region regToProcess)
		:m_regToProcess(regToProcess)
		,mAmbientOcclusionVolume(ambientOcclusionVolume)
		,mVolume(volume)
	{
		if(mThresholdVolume == 0)
		{
			mThresholdVolume = new PolyVox::RawVolume<PolyVox::Density8>(Region(0,0,0,127,127,127));
		}

		if(mBlurredVolume == 0)
		{
			mBlurredVolume = new PolyVox::RawVolume<PolyVox::Density8>(Region(0,0,0,127,127,127));
		}
	}
	
	void AmbientOcclusionTask::run(void)
	{	
		bool calcuateAmbientOcclusion = true;
		if(calcuateAmbientOcclusion)
		{
			for(uint32_t z = 0; z < mThresholdVolume->getDepth(); z++)
			{
				for(uint32_t y = 0; y < mThresholdVolume->getHeight(); y++)
				{
					for(uint32_t x = 0; x < mThresholdVolume->getWidth(); x++)
					{
						if(mVolume->getVoxelAt(x,y,z).getMaterial() == 0)
						{
							mThresholdVolume->setVoxelAt(x,y,z,255);
						}
						else
						{
							mThresholdVolume->setVoxelAt(x,y,z,0);
						}
					}
				}
			}
			//Halfway between 0 and 255, so the surrounding neither brightens nor darkens.
			mThresholdVolume->setBorderValue(128);

			LowPassFilter<RawVolume, RawVolume, Density8> pass1(mThresholdVolume, Region(0,0,0,127,127,127), mBlurredVolume, Region(0,0,0,127,127,127), 7);

			QTime time;
			time.start();

			pass1.executeSAT();

			qDebug() << "Ran executeSAT() in " << time.elapsed() << "ms";

			for(uint32_t z = 0; z < mAmbientOcclusionVolume->getDimension(2); z++)
			{
				for(uint32_t y = 0; y < mAmbientOcclusionVolume->getDimension(1); y++)
				{
					for(uint32_t x = 0; x < mAmbientOcclusionVolume->getDimension(0); x++)
					{
						(*mAmbientOcclusionVolume)[z][y][x] = mBlurredVolume->getVoxelAt(x,y,z).getDensity();

						//(*mAmbientOcclusionVolume)[z][y][x] = x * 2;
					}
				}
			}
		}
		else
		{
			int fillValue = 160; //A kind of average occlusion, given that most parts are not occluded.
			std::fill(mAmbientOcclusionVolume->getRawData(), mAmbientOcclusionVolume->getRawData() + mAmbientOcclusionVolume->getNoOfElements(), fillValue);
		}

		emit finished(this);
		return;
	}
}
