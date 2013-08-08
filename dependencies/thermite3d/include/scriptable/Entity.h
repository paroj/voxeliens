#ifndef ENTITY_H_
#define ENTITY_H_

#include "RenderComponent.h"

#include "OgrePrerequisites.h"

#include <QString>

namespace Thermite
{
	class Entity : public RenderComponent
	{
		Q_OBJECT

	public:
		Entity(const QString& meshName, const QString& materialName, Object* parent = 0);
		~Entity();

		Q_PROPERTY(QString meshName READ meshName WRITE setMeshName)
		Q_PROPERTY(QString materialName READ materialName WRITE setMaterialName)

		const QString& meshName(void) const;
		void setMeshName(const QString& name);

		const QString& materialName(void) const;
		void setMaterialName(const QString& name);

		bool castsShadows(void) const;
		void setCastsShadows(bool value);

		void update(void);

	public:
		QString mMeshName;
		QString mMaterialName;

		bool mCastsShadows;

		bool mUpdateMesh;
		bool mUpdateMaterial;
		bool mUpdateCastsShadows;

		Ogre::Entity* mOgreEntity;
	};
}


#endif //ENTITY_H_