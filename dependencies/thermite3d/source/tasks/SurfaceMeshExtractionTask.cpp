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

#include "SurfaceMeshExtractionTask.h"

#include "PolyVoxCore/Material.h"

#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

#include <QMutex>

using namespace PolyVox;

namespace Thermite
{
	SurfaceMeshExtractionTask::SurfaceMeshExtractionTask(PolyVox::SimpleVolume<PolyVox::Material16>* volume, PolyVox::Region regToProcess, uint32_t uTimeStamp)
		:m_regToProcess(regToProcess)
		,m_uTimeStamp(uTimeStamp)
		,mVolume(volume)
	{
	}
	
	void SurfaceMeshExtractionTask::run(void)
	{
		//This is bad - can we make SurfaceExtractor reenterant (?) and just have one which all runnables share?
		//Or at least not use 'new'
		PolyVox::CubicSurfaceExtractor<SimpleVolume, Material16> surfaceExtractor(mVolume, m_regToProcess, &m_meshResult);
		
		surfaceExtractor.execute();
		//computeNormalsForVertices(m_pGameLogic->mMap->volumeResource->getVolume(),*(m_taskData.m_meshResult.get()), PolyVox::SOBEL_SMOOTHED);
		//m_taskData.m_meshResult->generateAveragedFaceNormals(true);

		/*PolyVox::SurfaceMesh<PolyVox::PositionMaterial> meshDecimated;
		PolyVox::MeshDecimator<PositionMaterial> decimator(&m_meshResult, &meshDecimated);
		decimator.execute();
		m_meshResult = meshDecimated;*/

		emit finished(this);
	}
}
