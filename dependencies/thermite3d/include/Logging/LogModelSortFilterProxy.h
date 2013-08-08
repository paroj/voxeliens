#ifndef THERMITE_LOGMODELSORTFILTERPROXY_H_
#define THERMITE_LOGMODELSORTFILTERPROXY_H_

#include <QSortFilterProxyModel>

namespace Thermite
{
	class LogModelSortFilterProxy : public QSortFilterProxyModel
	{
		Q_OBJECT
	public:
		LogModelSortFilterProxy(QObject *parent = 0);
		void setShowLineAndFile(bool show);
		void setVisisbleLevels(int levelBitmask);

	protected:
		bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
		bool filterAcceptsColumn(int sourceColumn, const QModelIndex &sourceParent) const;

	private:
		bool mShowLineAndFile;
		int mVisibleLevels;
	};
}
#endif