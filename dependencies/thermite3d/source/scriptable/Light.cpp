#include "Light.h"

#include "Utility.h"

#include "OgreEntity.h"
#include "OgreRoot.h"

namespace Thermite
{
	Light::Light(Object* parent)
		:RenderComponent(parent)
		,m_colColour(255,255,255)
		,mType(PointLight)
		,mDirectionalFixupSceneNode(0)
		,mOgreLight(0)
	{
	}

	Light::~Light()
	{
		if(mDirectionalFixupSceneNode)
		{
			if(mOgreLight)
			{
				mDirectionalFixupSceneNode->detachObject(mOgreLight);
			}
			mSceneManager->destroyLight(mOgreLight);
			mSceneManager->destroySceneNode(mDirectionalFixupSceneNode);
		}
	}

	void Light::update(void)
	{
		RenderComponent::update();

		std::string objAddressAsString = QString::number(reinterpret_cast<qulonglong>(mParent), 16).toStdString();

		//In general Ogre considers the negative z'axis to be the forward direction. This can be seen with cameras (which point along negative z)
		//and also with SceneNodes (see here: http://www.ogre3d.org/docs/api/html/classOgre_1_1SceneNode.html#a4a6e34aab331802bc836668e78a08508).
		//However, when it comes to directional lights the seem to emit light along the positive Z, rather than negative. This is the observed behaviour,
		//although no documents have been found. As a result we insert this fixup node to reverse the direction, so they are consistant with other types.
		if(mDirectionalFixupSceneNode == 0)
		{
			std::string sceneNodeName(objAddressAsString + "_DirectionalFixupSceneNode");
			mDirectionalFixupSceneNode = mOgreSceneNode->createChildSceneNode(sceneNodeName);
			mDirectionalFixupSceneNode->yaw(Ogre::Radian(3.14159265));
		}

		if(!mOgreLight)
		{
			Ogre::SceneManager* sceneManager = Ogre::Root::getSingletonPtr()->getSceneManager("OgreSceneManager");
			mOgreLight = sceneManager->createLight(objAddressAsString + "_Light");
			//Ogre::Entity* ogreEntity = sceneManager->createEntity(generateUID("PointLight Marker"), "Icosphere5.mesh");
			mDirectionalFixupSceneNode->attachObject(mOgreLight);
			//mDirectionalFixupSceneNode->attachObject(ogreEntity);
		}

		switch(getType())
		{
		case Light::PointLight:
			mOgreLight->setType(Ogre::Light::LT_POINT);
			mOgreLight->setAttenuation(1000, 0.0, 0.001, 0.0);
			break;
		case Light::DirectionalLight:
			mOgreLight->setType(Ogre::Light::LT_DIRECTIONAL);
			mOgreLight->setAttenuation(1000, 1.0, 1.0, 1.0);
			break;
		case Light::SpotLight:
			mOgreLight->setType(Ogre::Light::LT_SPOTLIGHT);
			break;
		}

		QColor col = getColour();
		mOgreLight->setDiffuseColour(col.redF(), col.greenF(), col.blueF());
		mOgreLight->setSpecularColour(col.redF(), col.greenF(), col.blueF());		
	}

	const QColor& Light::getColour(void) const
	{
		return m_colColour;
	}

	void Light::setColour(const QColor& col)
	{
		m_colColour = col;
	}

	Light::LightType Light::getType(void) const
	{
		return mType;
	}

	void Light::setType(Light::LightType type)
	{
		mType = type;
	}
}
