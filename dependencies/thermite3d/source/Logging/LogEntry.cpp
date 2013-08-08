#include "LogEntry.h"

namespace Thermite
{
	LogEntry::LogEntry(const QString &msg, LogLevel level)
		:mMsg(msg)
		,mLevel(level)
		,mTimestamp(QTime::currentTime())
	{
	}

	LogLevel LogEntry::getLevel(void)
	{
		return mLevel;
	}

	const QString& LogEntry::getMessage(void)
	{
		return mMsg;
	}

	const QTime& LogEntry::getTimestamp(void)
	{
		return mTimestamp;
	}
}