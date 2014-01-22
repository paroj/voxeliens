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

#ifndef __TextResource_H__
#define __TextResource_H__

#include "OgrePrerequisites.h"
#include "OgreSharedPtr.h"

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"
#include "PolyVoxImpl/TypeDef.h"

#include <OgreResourceManager.h>

namespace Thermite
{
	class TextResource : public Ogre::Resource
	{
	public:		
		TextResource (Ogre::ResourceManager *creator, const Ogre::String &name, 
			Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
			Ogre::ManualResourceLoader *loader = 0);
		~TextResource();		

		std::string getTextData(void) const;

	protected:

		// must implement these from the Ogre::Resource interface
		void loadImpl ();
		void unloadImpl ();
		size_t calculateSize () const;

	private:
		std::string mTextData;
	};

	class TextResourcePtr : public Ogre::SharedPtr<TextResource> 
	{
	public:
		TextResourcePtr () : Ogre::SharedPtr<TextResource> () {}
		explicit TextResourcePtr (TextResource *rep) : Ogre::SharedPtr<TextResource> (rep) {}
		TextResourcePtr (const TextResourcePtr &r) : Ogre::SharedPtr<TextResource> (r) {} 
		TextResourcePtr (const Ogre::ResourcePtr &r) : Ogre::SharedPtr<TextResource> ()
		{
			if(r.isNull())
				return;

			// lock & copy other mutex pointer
			pRep = static_cast<TextResource*> (r.getPointer ());
		}

		/// Operator used to convert a ResourcePtr to a TextResourcePtr
		TextResourcePtr& operator=(const Ogre::ResourcePtr& r)
		{
			if (pRep == static_cast<TextResource*> (r.getPointer ()))
				return *this;
			release ();

			if(r.isNull())
				return *this;

			// lock & copy other mutex pointer
			pRep = static_cast<TextResource*> (r.getPointer());
			return *this;
		}
	};
}

#endif //__TextResource_H__
