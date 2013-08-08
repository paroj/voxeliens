#ifndef RENDER_COMPONENT_H_
#define RENDER_COMPONENT_H_

#include "Component.h"

#include "OgreSceneNode.h"

namespace Thermite
{
	class RenderComponent : public Component
	{
	public:
		RenderComponent(Object* parent);
		~RenderComponent(void);

		void onEnabled(bool enabled);

		void update(void);

	public:
		Ogre::SceneManager* mSceneManager;
		Ogre::SceneNode* mOgreSceneNode;

		bool mIsVisible;
		bool mUpdateIsVisible;
	};
}

#endif //RENDER_COMPONENT_H_
