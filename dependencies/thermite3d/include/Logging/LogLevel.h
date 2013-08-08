#ifndef THERMITE_LOGLEVEL_H_
#define THERMITE_LOGLEVEL_H_

#include <QMetaType>

namespace Thermite
{
	enum LogLevel
	{ 
		//These values are specifically chosen to be over 1000. This means they can be used as the 'type'
		//of a QTableWidgetItem. This lets us easily identify the message type of any given row.
		//In addition, they are powers of two in order to simplify the logic when filtering messages.
		LL_DEBUG = 1024,
		LL_INFO = 2048,
		LL_WARNING = 4096,
		LL_ERROR = 8192
	};
}

//We want to be able to pass log messages across QT
//threads, which means making the LogLevel an Qt MetaType.
Q_DECLARE_METATYPE(Thermite::LogLevel)

#endif
