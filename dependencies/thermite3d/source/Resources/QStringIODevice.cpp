#include "QStringIODevice.h"

namespace Thermite
{
	QStringIODevice::QStringIODevice(const QString& string, QObject* parent)
		:QIODevice(parent)
		,mString(string)
		,mCurrent(0)
	{
	}

	qint64	QStringIODevice::readData(char *data, qint64 maxSize)
	{
		//Check there is data available
		qint64 bytesAvailable = mString.length() - mCurrent;
		if(bytesAvailable == 0)
		{
			return -1;
		}

		//Copy the data
		qint64 bytesToRead = qMin(bytesAvailable, maxSize);
		memcpy(data, mString.toStdString().c_str() + mCurrent, bytesToRead);
		mCurrent += bytesToRead;

		//Return the number of bytes read.
		return bytesToRead;
	}

	qint64	QStringIODevice::writeData(const char *data, qint64 maxSize)
	{
		Q_ASSERT(false);
		return -1; //Error
	}
}