#ifndef LIGHT_H_
#define LIGHT_H_

#include "RenderComponent.h"

#include <QColor>
#include <QVector3D>

namespace Thermite
{
	class Light : public RenderComponent
	{
		Q_OBJECT

		Q_ENUMS(LightType)

		Q_PROPERTY(QColor colour READ getColour WRITE setColour)
		Q_PROPERTY(LightType type READ getType WRITE setType)		

	public:		

		enum LightType
		{
			PointLight = 0,
			DirectionalLight = 1,
			SpotLight = 2
		};

		Light(Object* parent = 0);
		~Light();

		void update(void);

		const QColor& getColour(void) const;
		void setColour(const QColor& col);

		LightType getType(void) const;
		void setType(LightType type);

	private:
		QColor m_colColour;
		LightType mType;

		Ogre::SceneNode* mDirectionalFixupSceneNode;
		Ogre::Light* mOgreLight;
	};
}

#endif //LIGHT_H_