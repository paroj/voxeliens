#include "Entity.h"

#include "OgreEntity.h"
#include "OgreRoot.h"

namespace Thermite
{
	Entity::Entity(const QString& meshName, const QString& materialName, Object* parent)
		:RenderComponent(parent)
		,mOgreEntity(0)
		,mUpdateMesh(false)
		,mUpdateMaterial(false)
		,mUpdateCastsShadows(false)
	{
		setMaterialName(materialName);
		setMeshName(meshName);
	}
	
	Entity::~Entity()
	{
		if(mOgreEntity)
		{
			mOgreSceneNode->detachObject(mOgreEntity);
			mSceneManager->destroyMovableObject(mOgreEntity);
		}
	}

	const QString& Entity::meshName(void) const
	{
		return mMeshName;
	}

	void Entity::setMeshName(const QString& name)
	{
		mMeshName = name;
		mUpdateMesh = true;
	}

	const QString& Entity::materialName(void) const
	{
		return mMaterialName;
	}

	void Entity::setMaterialName(const QString& name)
	{
		mMaterialName = name;
		mUpdateMaterial = true;
	}

	bool Entity::castsShadows(void) const
	{
		return mCastsShadows;
	}

	void Entity::setCastsShadows(bool value)
	{
		mCastsShadows = value;
		mUpdateCastsShadows = true;
	}

	void Entity::update(void)
	{
		RenderComponent::update();	

		if(mUpdateMesh)
		{
			//Delete the old ogre entity first
			if(mOgreEntity)
			{
				mOgreSceneNode->detachObject(mOgreEntity);
				mSceneManager->destroyMovableObject(mOgreEntity);
			}

			if(mMeshName.isEmpty() == false)
			{
				std::string objAddressAsString = QString::number(reinterpret_cast<qulonglong>(mParent), 16).toStdString();
				std::string entityName(objAddressAsString + "_Entity");
				mOgreEntity = mSceneManager->createEntity(entityName, meshName().toAscii().constData());
				mOgreSceneNode->attachObject(mOgreEntity);
			}

			mUpdateMesh = false;
		}

		//Set a custom material if necessary
		if(mUpdateMaterial)
		{
			if(materialName().isEmpty() == false)
			{
				if(mOgreEntity)
				{
					//NOTE: Might be sensible to check if this really need setting, perhaps it is slow.
					//But you can only get materials from SubEntities.
					mOgreEntity->setMaterialName(materialName().toAscii().constData());
				}
			}

			mUpdateMaterial = false;
		}

		if(mUpdateCastsShadows)
		{
			if(mOgreEntity)
			{
				mOgreEntity->setCastShadows(mCastsShadows);
			}
		}
	}
}
