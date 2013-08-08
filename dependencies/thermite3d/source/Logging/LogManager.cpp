#include "LogManager.h"

#include <QDir>
#include <QTabWidget>

namespace Thermite
{
	LogManager::LogManager(QWidget *parent)
		:QDialog(parent,Qt::Tool)
	{
		mGridLayout = new QGridLayout;
		mLogTabs = new QTabWidget;
		mGridLayout->addWidget(mLogTabs);
		setLayout(mGridLayout);

		setModal(false);

		//This makes sure the images are present so that the HTML logs display correctly.
		QPixmap debugImage = QPixmap(QString::fromUtf8(":/images/icons/script-error.png"));
		QPixmap infoImage = QPixmap(QString::fromUtf8(":/images/icons/dialog-information.png"));
		QPixmap warningImage = QPixmap(QString::fromUtf8(":/images/icons/dialog-warning.png"));
		QPixmap errorImage = QPixmap(QString::fromUtf8(":/images/icons/dialog-error.png"));

		QDir::current().mkdir("logs");
		QDir::current().mkdir("logs/images");
		debugImage.save("logs/images/script-error.png");
		infoImage.save("logs/images/dialog-information.png");
		warningImage.save("logs/images/dialog-warning.png");
		errorImage.save("logs/images/dialog-error.png");

	}

	Log* LogManager::createLog(const QString& name)
	{
		Log* mLog = new Log(name, this);
		mLogs.insert(name, mLog);
		mLogTabs->addTab(mLog, name);
		return mLog;
	}

	Log* LogManager::getLogByName(const QString& name)
	{
		//We do an explicit check, as otherwise the operator[] will insert an absent key.
		if(mLogs.contains(name))
		{
			return mLogs[name];
		}
		else
		{
			return 0;
		}
	}

	void LogManager::setForceProcessEvents(bool forceProcessEvents)
	{
		foreach (Log* value, mLogs)
			value->setForceProcessEvents(forceProcessEvents);
	}

	void LogManager::setVisibleLog(Log* log)
	{
		mLogTabs->setCurrentWidget(log);
	}
}
