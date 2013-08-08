#include "SkyBox.h"

#include "OgreRoot.h"

namespace Thermite
{
	SkyBox::SkyBox(Object* parent)
		:RenderComponent(parent)
	{
		mMaterialName = "";
	}

	void SkyBox::update(void)
	{
		RenderComponent::update();
	}

	const QString& SkyBox::materialName(void) const
	{
		return mMaterialName;
	}

	void SkyBox::setMaterialName(const QString& name)
	{
		mMaterialName = name;

		Ogre::SceneManager* sceneManager = Ogre::Root::getSingletonPtr()->getSceneManager("OgreSceneManager");
		sceneManager->setSkyBox(true, materialName().toAscii().constData(), 2500);
	}
}
