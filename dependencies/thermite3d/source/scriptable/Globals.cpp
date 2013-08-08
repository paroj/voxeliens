#include "Globals.h"

#include <QMutex>
#include <QTime>

namespace Thermite
{
	//Our single globals object
	Globals globals;

	Globals::Globals(QObject* parent)
		:QObject(parent)
		,mTimeStamp(0)
		,mTimeStampMutex(0)
	{
		mTimeSinceAppStart = new QTime;
		mTimeSinceAppStart->start();

		mTimeStampMutex = new QMutex;
	}

	Globals::~Globals()
	{
		if(mTimeStampMutex)
		{
			delete mTimeStampMutex;
		}
	}

	int Globals::timeSinceAppStart(void) const
	{
		return mTimeSinceAppStart->elapsed();
	}

	uint32_t Globals::timeStamp(void)
	{
		mTimeStampMutex->lock();
		++mTimeStamp;
		mTimeStampMutex->unlock();
		return mTimeStamp;
	}
}
