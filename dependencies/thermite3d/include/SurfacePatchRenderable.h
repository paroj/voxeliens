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

#ifndef __SurfacePatchRenderable_H__
#define __SurfacePatchRenderable_H__

#include "Ogre.h"
#include <vector>

#include "PolyVoxCore/SurfaceMesh.h"

namespace Thermite
{
	//IDEA - If profiling identifies this class as a bottleneck, we could implement a memory pooling system.
	//All buffers could be powers of two, and we get the smallest one which is big enough for our needs.
	//See http://www.ogre3d.org/wiki/index.php/DynamicGrowingBuffers
	class SurfacePatchRenderable : public Ogre::MovableObject, public Ogre::Renderable
	{
	public:
		SurfacePatchRenderable(const Ogre::String& strName);
		~SurfacePatchRenderable(void);

		const Ogre::AxisAlignedBox& getBoundingBox(void) const;
		Ogre::Real getBoundingRadius(void) const;
		const Ogre::LightList& getLights(void) const;
		const Ogre::MaterialPtr& getMaterial(void) const;
		const Ogre::String& getMovableType(void) const;
		void getRenderOperation(Ogre::RenderOperation& op);
		Ogre::Real getSquaredViewDepth(const Ogre::Camera *cam) const;
		const Ogre::Quaternion &getWorldOrientation(void) const;
		const Ogre::Vector3 &getWorldPosition(void) const;
		void getWorldTransforms( Ogre::Matrix4* xform ) const;	

		bool isSingleMaterial(void);

		void setBoundingBox( const Ogre::AxisAlignedBox& box );
		void setMaterial( const Ogre::String& matName );
		void setWorldTransform( const Ogre::Matrix4& xform );
		
		virtual void _updateRenderQueue(Ogre::RenderQueue* queue);
		void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false);

		static Ogre::Real* addVertex(const PolyVox::PositionMaterialNormal& vertex, float alpha, Ogre::Real* prPos);
		void buildRenderOperationFrom(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, bool bSingleMaterial);
		void buildRenderOperationFrom(PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& mesh);

	protected:
		Ogre::RenderOperation* m_RenderOp;
		Ogre::Matrix4 m_matWorldTransform;
		Ogre::AxisAlignedBox mBox;
		Ogre::String m_strMatName;
		Ogre::MaterialPtr m_pMaterial;
		Ogre::SceneManager *m_pParentSceneManager;

		bool m_bIsSingleMaterial;

	public:
		Ogre::Vector3 m_v3dPos;

		std::vector<PolyVox::LodRecord> m_vecLodRecords;

		mutable Ogre::LightList list;
	};

	/** Factory object for creating Light instances */
	class SurfacePatchRenderableFactory : public Ogre::MovableObjectFactory
	{
	protected:
		Ogre::MovableObject* createInstanceImpl( const Ogre::String& name, const Ogre::NameValuePairList* params);
	public:
		SurfacePatchRenderableFactory() {}
		~SurfacePatchRenderableFactory() {}

		static Ogre::String FACTORY_TYPE_NAME;

		const Ogre::String& getType(void) const;
		void destroyInstance( Ogre::MovableObject* obj);  

	};
}

#endif /* __SurfacePatchRenderable_H__ */
