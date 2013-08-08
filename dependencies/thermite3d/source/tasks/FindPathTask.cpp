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

#include "FindPathTask.h"

#include "PolyVoxCore/Material.h"

using namespace PolyVox;
using namespace std;

namespace Thermite
{
	FindPathTask::FindPathTask(PolyVox::SimpleVolume<PolyVox::Material16>* polyVoxVolume, QVector3D start, QVector3D end, Volume* thermiteVolume)
		:mPolyVoxVolume(polyVoxVolume)
		,mStart(start)
		,mEnd(end)
		,mThermiteVolume(thermiteVolume)
	{
	}

	void FindPathTask::run(void)
	{
		Vector3DInt32 start(mStart.x() + 0.5, mStart.y() + 0.5, mStart.z() + 0.5);
		Vector3DInt32 end(mEnd.x() + 0.5, mEnd.y() + 0.5, mEnd.z() + 0.5);

		list<Vector3DInt32> path;
		TankWarsVoxelValidator<Material16> validator(start.getY());
		AStarPathfinderParams<SimpleVolume, Material16> pathfinderParams(mPolyVoxVolume, start, end, &path, 2.0f, 10000);
		pathfinderParams.connectivity = TwentySixConnected;
		pathfinderParams.isVoxelValidForPath = validator;
		AStarPathfinder<SimpleVolume, Material16> pathfinder(pathfinderParams);
		try
		{
			pathfinder.execute();
		}
		catch(runtime_error&) {} //No path found

		QVariantList variantPath;

		int ct = 0;
		int howOftenToInclude = 10; //Controls whether we include every xth point.
		list<Vector3DInt32>::iterator iter;
		for(iter = path.begin(); iter != path.end(); iter++)
		{
			if(ct % howOftenToInclude == 0)
			{
				variantPath.append(QVector3D(iter->getX(), iter->getY(), iter->getZ()));
			}
			ct++;
		}

		if(path.size() != 0)
		{
			if((ct-1) % howOftenToInclude != 0)
			{
				//We didn't add the end point, add it now
				iter--;
				variantPath.append(QVector3D(iter->getX(), iter->getY(), iter->getZ()));
			}
		}

		emit finished(variantPath);
	}
}
