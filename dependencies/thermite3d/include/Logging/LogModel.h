#ifndef THERMITE_LOGMODEL_H_
#define THERMITE_LOGMODEL_H_

#include "LogLevel.h"
#include "LogEntry.h"

#include <QAbstractTableModel>

namespace Thermite
{
	class Log;
	class LogEntry;

	const int LOG_COLUMN_COUNT = 4; //Do we really need this?
	const int LOG_FILTER_COLUMN = 3; //Do we really need this?

	// A simple model that will let us customize display, sorting, and filtering of log messages
	class LogModel : public QAbstractTableModel
	{
		Q_OBJECT

	public:
		// takes a pointer to the owning log object (right now just for fetching icons & colors)
		LogModel(Log *log, QObject *parent = 0);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	public slots:
		//TODO: make slots for debug, warning, etc...
		void append(const LogEntry& logEntry);

	private:
		QList<LogEntry> mEntries;
		Log *mLog;
	};
}
#endif
