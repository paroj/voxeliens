#include "LogModelSortFilterProxy.h"

namespace Thermite
{
	LogModelSortFilterProxy::LogModelSortFilterProxy(QObject *parent)
		:QSortFilterProxyModel(parent)
		,mShowLineAndFile(false) // TODO: add a GUI switch for this (right now nothing sends file/line info) so it's off
	{
		setFilterCaseSensitivity(Qt::CaseInsensitive);
	}

	bool LogModelSortFilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
	{		
		// both of these must be true to show this row
		bool passedLevelTest = false;
		bool passedTextTest = false;

		// go find the log level of the entry in the real model
		QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);
		QVariant data = sourceModel()->data(idx, Qt::UserRole);

		// does it fit in our bitmask?
		if (data.toInt() & mVisibleLevels)
		{
			passedLevelTest = true;
		}
		// if it passed the bitmask, then make sure the text matches
		if (passedLevelTest) 
		{
			passedTextTest = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
		}

		return passedLevelTest && passedTextTest; // has to match both
	}

	bool LogModelSortFilterProxy::filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const
	{
		if (!mShowLineAndFile && sourceColumn == 1 || sourceColumn == 2)
		{
			return false;
		} 
		else
		{
			return QSortFilterProxyModel::filterAcceptsColumn(sourceColumn, sourceParent);
		}
	}

	void LogModelSortFilterProxy::setVisisbleLevels(int levelBitmask)
	{
		mVisibleLevels = levelBitmask;
		invalidateFilter();
	}
}