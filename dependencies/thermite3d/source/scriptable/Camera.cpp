#include "Camera.h"

namespace Thermite
{
	Camera::Camera(Object * parent)
		:Object(parent)
	{
		mFieldOfView = 1.0;
	}

	float Camera::fieldOfView(void) const
	{
		return mFieldOfView;
	}

	void Camera::setFieldOfView(float fieldOfView)
	{
		mFieldOfView = fieldOfView;
	}
}
