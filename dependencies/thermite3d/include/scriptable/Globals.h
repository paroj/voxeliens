#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "QtForwardDeclarations.h"

#include <QObject>

#include <cstdint>

namespace Thermite
{
	class Globals : public QObject
	{
		Q_OBJECT

	public:
		Globals(QObject* parent = 0);
		~Globals();

		Q_PROPERTY(int timeSinceAppStart READ timeSinceAppStart)
		Q_PROPERTY(uint32_t timeStamp READ timeStamp)

	public slots:
		int timeSinceAppStart(void) const;
		uint32_t timeStamp(void);

	private:
		QTime* mTimeSinceAppStart;

		QMutex* mTimeStampMutex;
		uint32_t mTimeStamp;
	};

	extern Globals globals;
}

#endif //GLOBALS_H_