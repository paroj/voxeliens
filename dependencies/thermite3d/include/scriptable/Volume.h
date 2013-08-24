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

#ifndef __THERMITE_VOLUME_H__
#define __THERMITE_VOLUME_H__

#include "Object.h"
#include "QtForwardDeclarations.h"
#include "ThermiteForwardDeclarations.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SimpleVolume.h"

#include "OgrePrerequisites.h"
#include "OgreTexture.h"

#include <QVariantList>

#include <map>
#include <set>

#include "PolyVoxCore/Vector.h"

namespace Thermite
{
	class Volume : public QObject
	{
		Q_OBJECT

	public:
		Volume(uint32_t width, uint32_t height, uint32_t depth, Object* parent = 0);
		~Volume(void);

		void setPolyVoxVolume(PolyVox::SimpleVolume<PolyVox::Material16>* pPolyVoxVolume, uint16_t regionSideLength);

		void initialise(void);
		void update(void);
		void updatePolyVoxGeometry(const QVector3D& cameraPos);

		bool readFromFile(const QString& path);
		bool writeToFile(const QString& path);

	signals:
		void foundPath(QVariantList path);

	public slots:

		void uploadSurfaceMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& mesh, PolyVox::Region region, Volume& volume);		
		void addSurfacePatchRenderable(std::string materialName, PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& mesh, PolyVox::Region region);

		void uploadSurfaceMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, PolyVox::Region region, Volume& volume);		
		void addSurfacePatchRenderable(std::string materialName, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, PolyVox::Region region);

		//Deletes all children (both nodes and attached objects) but not the node itself.
		void deleteSceneNodeChildren(Ogre::SceneNode* sceneNode);

		void createVerticalHole(int xStart, int yStart, int zStart, int yEnd);
		void createCuboidAt(QVector3D centre, QVector3D dimensions, int material, bool bPaintMode);
		void createSphereAt(QVector3D centre, float radius, int material, bool bPaintMode);
		QPair<bool, QVector3D> getRayVolumeIntersection(QVector3D rayOrigin, const QVector3D& rayDir);
		int materialAtPosition(QVector3D position);

		void finishedHandler(QVariantList path);

		void findPath(QVector3D start, QVector3D end);


		bool loadFromFile(const QString& filename);

		void uploadSurfaceExtractorResult(SurfaceMeshExtractionTask* pTask);
		void uploadSurfaceDecimatorResult(SurfaceMeshDecimationTask* pTask);

	public:
		//static TaskProcessorThread* m_backgroundThread;

		Ogre::SceneNode* mVolumeSceneNode;

		PolyVox::Array<3, uint32_t> mVolLastUploadedTimeStamps;
		uint16_t mCachedVolumeWidthInRegions;
		uint16_t mCachedVolumeHeightInRegions;
		uint16_t mCachedVolumeDepthInRegions;

		PolyVox::Array<3, Ogre::SceneNode*> m_volOgreSceneNodes;

		bool mMultiThreadedSurfaceExtraction;

		PolyVox::SimpleVolume<PolyVox::Material16>* m_pPolyVoxVolume;
		uint16_t mRegionSideLength;
		uint16_t mVolumeWidthInRegions;
		uint16_t mVolumeHeightInRegions;
		uint16_t mVolumeDepthInRegions;

		std::map< std::string, std::set<uint8_t> > m_mapMaterialIds;	

		
		PolyVox::Array<3, PolyVox::SurfaceMesh<PolyVox::PositionMaterial>*> m_volSurfaceMeshes;
		PolyVox::Array<3, uint32_t> mLastModifiedArray;
		PolyVox::Array<3, uint32_t> mExtractionStartedArray;
		PolyVox::Array<3, uint32_t> mExtractionFinishedArray;
		PolyVox::Array<3, bool> mRegionBeingExtracted;
		//PolyVox::Array<3, SurfaceMeshDecimationTask*> m_volSurfaceDecimators;

		bool mIsModified;

	public:
		bool isRegionBeingExtracted(const PolyVox::Region& regionToTest);
		void updateLastModifedArray(const PolyVox::Region& regionToTest);
	};	
}


#endif //__THERMITE_VOLUME_H__