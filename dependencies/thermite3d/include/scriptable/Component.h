#ifndef COMPONENT_H_
#define COMPONENT_H_

#include "Object.h"

#include <QObject>

namespace Thermite
{
	class Component : public QObject
	{
		Q_OBJECT

	public:
		Component(Object* parent);
		virtual ~Component(void);

		Q_PROPERTY(bool isEnabled READ isEnabled WRITE setEnabled)

		bool isEnabled(void) const;
		void setEnabled(bool isEnabled);

		virtual void onEnabled(bool enabled) {}

		virtual void update(void);

		Object* mParent;

	private:
		bool mIsEnabled;
	};
}

#endif //COMPONENT_H_
