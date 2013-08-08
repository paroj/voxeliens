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

#ifndef __THERMITE_AMBIENT_OCCLUSION_TASK_H__
#define __THERMITE_AMBIENT_OCCLUSION_TASK_H__

#include "Task.h"

#include <PolyVoxCore/Material.h>
#include <PolyVoxCore/PolyVoxForwardDeclarations.h>
#include <PolyVoxCore/RawVolume.h>
#include <PolyVoxCore/Region.h>
#include <PolyVoxCore/SimpleVolume.h>
#include <PolyVoxCore/SurfaceMesh.h>

namespace Thermite
{

	class AmbientOcclusionTask : public Task
	{
		Q_OBJECT
	public:
		AmbientOcclusionTask(PolyVox::SimpleVolume<PolyVox::Material16>* volume, PolyVox::Array<3, uint8_t>* ambientOcclusionVolume, PolyVox::Region regToProcess);

		void run(void);

	signals:
		void finished(AmbientOcclusionTask* pTask);

	public:
		PolyVox::Region m_regToProcess;
		PolyVox::Array<3, uint8_t>* mAmbientOcclusionVolume;
		PolyVox::SimpleVolume<PolyVox::Material16>* mVolume;

		static PolyVox::RawVolume<PolyVox::Density8>* mThresholdVolume;
		static PolyVox::RawVolume<PolyVox::Density8>* mBlurredVolume;
	};
}

#endif //__THERMITE_AMBIENT_OCCLUSION_TASK_H__