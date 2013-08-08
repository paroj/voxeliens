#include "LogModel.h"

#include "Log.h"
#include "LogEntry.h"

namespace Thermite
{
	LogModel::LogModel(Log *log, QObject *parent)
		:QAbstractTableModel(parent)
		,mEntries(QList<LogEntry>())
		,mLog(log)
	{
	}

	QVariant LogModel::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid()) // bad index
		{
			return QVariant();
		}

		if (index.row() >= mEntries.size()) // row that we don't have in our model
		{
			return QVariant();
		}

		// valid index, so go fetch the entry object
		LogEntry entry = mEntries.at(index.row());
		switch (role)
		{
			case Qt::DecorationRole: // called to show icons
				// we only want to decorate 1 column, so the icon doesn't show up for every piece of data
				if (index.column() == 0)
				{ 
					// re-use the same QIcons that the Log owner created
					return mLog->getIcon(entry.getLevel());
				}
				else
				{
					return QVariant();
				}
			case Qt::DisplayRole:
				if(index.column() == 0)
				{
					return QVariant(entry.getTimestamp());
				}
				else if(index.column() == 3)
				{
					return QVariant(entry.getMessage());
				}
			case Qt::ForegroundRole:
				return mLog->getForegroundColour(entry.getLevel());
			case Qt::UserRole:
				return entry.getLevel();
			case Qt::ToolTipRole:
				return QVariant(entry.getMessage());
			default: //some other role we don't really care about
				return QVariant();
		}
	}

	int LogModel::rowCount(const QModelIndex &parent) const
	{
		return mEntries.length();
	}

	int LogModel::columnCount(const QModelIndex &parent) const
	{
		return LOG_COLUMN_COUNT;
	}

	QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		{
			return QVariant();
		}

		switch (section)
		{
			case 0:
				return QVariant("Time");
			case 1:
				return QVariant("Line");
			case 2:
				return QVariant("File");
			case 3:
				return QVariant("Message");
			default:
				return QVariant("Unset");
		}
	}

	void LogModel::append(const LogEntry& logEntry)
	{		
		//TODO: we could buffer appends up and only emit the change signals every 
		//Nth entry or every few secondsthis would probably be more efficient
		mEntries.append(logEntry);

		// if we don't emit these signals, the view never knows to update
		emit layoutAboutToBeChanged(); 
		emit layoutChanged(); // causes a redraw
	}
}