#include "RenderComponent.h"

#include "OgreRoot.h"

namespace Thermite
{
	RenderComponent::RenderComponent(Object* parent)
		:Component(parent)
		,mOgreSceneNode(0)
		,mIsVisible(true)
		,mUpdateIsVisible(false)
	{
		std::string objAddressAsString = QString::number(reinterpret_cast<qulonglong>(mParent), 16).toStdString();
		std::string sceneNodeName(objAddressAsString + "_SceneNode");
		mSceneManager = Ogre::Root::getSingletonPtr()->getSceneManager("OgreSceneManager");
		mOgreSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode(sceneNodeName);
	}

	RenderComponent::~RenderComponent(void)
	{
		mSceneManager->destroySceneNode(mOgreSceneNode);
	}

	void RenderComponent::onEnabled(bool enabled)
	{
		//mOgreSceneNode->setVisible(enabled);
		mIsVisible = enabled;
		mUpdateIsVisible = true;
	}

	void RenderComponent::update(void)
	{
		QMatrix4x4 qtTransform = mParent->transform();
		Ogre::Matrix4 ogreTransform;
		for(int row = 0; row < 4; ++row)
		{
			Ogre::Real* rowPtr = ogreTransform[row];
			for(int col = 0; col < 4; ++col)
			{
				Ogre::Real* colPtr = rowPtr + col;
				*colPtr = qtTransform(row, col);
			}
		}

		mOgreSceneNode->setOrientation(ogreTransform.extractQuaternion());
		mOgreSceneNode->setPosition(ogreTransform.getTrans());

		QVector3D scale = mParent->size();
		mOgreSceneNode->setScale(Ogre::Vector3(scale.x(), scale.y(), scale.z()));

		//if(mUpdateIsVisible) //Don't know why this doesn't work...
		{
			mOgreSceneNode->setVisible(mIsVisible);
			mUpdateIsVisible = false;
		}
	}
}
