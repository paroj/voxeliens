#include "Component.h"

namespace Thermite
{
	Component::Component(Object* parent)
		:mParent(parent)
		,mIsEnabled(true)
	{
		parent->setComponent(this);
	}

	Component::~Component(void)
	{
	}

	bool Component::isEnabled(void) const
	{
		return mIsEnabled;
	}

	void Component::setEnabled(bool enabled)
	{
		mIsEnabled = enabled;
		onEnabled(mIsEnabled);
	}

	void Component::update(void)
	{
	}
}
