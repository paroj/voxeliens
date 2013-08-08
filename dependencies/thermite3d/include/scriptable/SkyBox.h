#ifndef SKYBOX_H_
#define SKYBOX_H_

#include "RenderComponent.h"

namespace Thermite
{
	class SkyBox : public RenderComponent
	{
		Q_OBJECT

	public:		
		SkyBox(Object* parent = 0);

		Q_PROPERTY(QString materialName READ materialName WRITE setMaterialName)

		void update(void);

		const QString& materialName(void) const;
		void setMaterialName(const QString& name);

	private:
		QString mMaterialName;
	};
}

#endif //SKYBOX_H_