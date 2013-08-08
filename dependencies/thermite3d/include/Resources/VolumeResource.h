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

#ifndef __VolumeResource_H__
#define __VolumeResource_H__

#include "OgrePrerequisites.h"
#include "OgreSharedPtr.h"

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxImpl/TypeDef.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <OgreResourceManager.h>

namespace Thermite
{
	class VolumeResource : public Ogre::Resource
	{
	public:		
		VolumeResource (Ogre::ResourceManager *creator, const Ogre::String &name, 
			Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
			Ogre::ManualResourceLoader *loader = 0);
		~VolumeResource();		

		PolyVox::SimpleVolume<PolyVox::Material16>* getVolume(void);

	protected:

		// must implement these from the Ogre::Resource interface
		void loadImpl ();
		void unloadImpl ();
		size_t calculateSize () const;

		PolyVox::SimpleVolume<PolyVox::Material16>* m_pVolume;
	};

	class VolumeResourcePtr : public Ogre::SharedPtr<VolumeResource> 
	{
	public:
		VolumeResourcePtr () : Ogre::SharedPtr<VolumeResource> () {}
		explicit VolumeResourcePtr (VolumeResource *rep) : Ogre::SharedPtr<VolumeResource> (rep) {}
		VolumeResourcePtr (const VolumeResourcePtr &r) : Ogre::SharedPtr<VolumeResource> (r) {} 
		VolumeResourcePtr (const Ogre::ResourcePtr &r) : Ogre::SharedPtr<VolumeResource> ()
		{
			if(r.isNull())
				return;

			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
				OGRE_COPY_AUTO_SHARED_MUTEX (r.OGRE_AUTO_MUTEX_NAME)
				pRep = static_cast<VolumeResource*> (r.getPointer ());
			pUseCount = r.useCountPointer ();
			useFreeMethod = r.freeMethod();
			if (pUseCount)
			{
				++ (*pUseCount);
			}
		}

		/// Operator used to convert a ResourcePtr to a VolumeResourcePtr
		VolumeResourcePtr& operator=(const Ogre::ResourcePtr& r)
		{
			if (pRep == static_cast<VolumeResource*> (r.getPointer ()))
				return *this;
			release ();

			if(r.isNull())
				return *this;

			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
				OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
				pRep = static_cast<VolumeResource*> (r.getPointer());
			pUseCount = r.useCountPointer ();
			useFreeMethod = r.freeMethod();
			if (pUseCount)
			{
				++ (*pUseCount);
			}
			return *this;
		}
	};
}

#endif
