#include "Log.h"
#include "LogEntry.h"
#include "LogModel.h"
#include "LogModelSortFilterProxy.h"

#include <QHeaderView>
#include <QTime>
#include <QFile>
#include <QScrollBar>
#include <QSortFilterProxyModel>

namespace Thermite
{
	Log::Log(const QString& name, QWidget* parent)
	:QWidget(parent)
	,mForceProcessEvents(false)
	,mName(name)
	,mLogModel(new LogModel(this, this))
	,mProxyModel(new LogModelSortFilterProxy(this))
	,m_bSliderPressed(false)
	{
		setupUi(this);
		
		//setup connections 
		connect(showDebugButton, SIGNAL(toggled(bool)), this, SLOT(computeVisibleMessageTypes(bool)));
		connect(showInformationButton, SIGNAL(toggled(bool)), this, SLOT(computeVisibleMessageTypes(bool)));
		connect(showWarningsButton, SIGNAL(toggled(bool)), this, SLOT(computeVisibleMessageTypes(bool)));
		connect(showErrorsButton, SIGNAL(toggled(bool)), this, SLOT(computeVisibleMessageTypes(bool)));
		connect(filterLineEdit, SIGNAL(textChanged(const QString&)), mProxyModel, SLOT(setFilterFixedString(const QString&)));
		connect(clearFilterButton, SIGNAL(pressed()), filterLineEdit, SLOT(clear()));

		connect(m_pLogTable->verticalScrollBar(), SIGNAL(sliderPressed(void)), this, SLOT(onSliderPressed(void)));
		connect(m_pLogTable->verticalScrollBar(), SIGNAL(sliderReleased(void)), this, SLOT(onSliderReleased(void)));

		// We need to make sure that log messages are only written from the main thread. In order to do this we make use of the Qt signal slot mechanism.
		// The log message function simply emits a signal, an Qt will ensure that this is delivered to the main thread in a safe manner. There may be better
		// or faster ways of achiving this behaviour but it seems to work for now.
		qRegisterMetaType<LogLevel>("LogLevel");
		connect(this, SIGNAL(_logMessageReceived(const QString&, LogLevel)), this, SLOT(logMessageImpl(const QString&, LogLevel)), Qt::QueuedConnection);
		
		//Create a sort/filter proxy of our log entries model so we can sort and filter it :)
		mProxyModel->setSourceModel(mLogModel); // the proxy should point to the real model
		mProxyModel->setFilterKeyColumn(LOG_FILTER_COLUMN); // the message column (for filtering by regex) TODO: nuke the MAGIC NUMBER!
		mProxyModel->setDynamicSortFilter(true); // keep filtering updated when data changes
		m_pLogTable->setModel(mProxyModel); // the view points to our proxy
		m_pLogTable->setSortingEnabled(false); // no need for sorting and it seems to get slow

		//Using this approach of resizing to contents seems to have
		//noticeable performance penalties when adding rows to the log.
		//m_pLogTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
		//m_pLogTable->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
		//m_pLogTable->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
		//m_pLogTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

		//So instead we hard code the sizes
		m_pLogTable->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
		m_pLogTable->horizontalHeader()->resizeSection(0, 100);
		m_pLogTable->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
		m_pLogTable->horizontalHeader()->resizeSection(1, 30);
		m_pLogTable->horizontalHeader()->resizeSection(2, 120);
		m_pLogTable->horizontalHeader()->resizeSection(3, 800);

		m_pLogTable->verticalHeader()->hide();
		//m_pLogTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
		//m_pLogTable->verticalHeader()->resizeSections(20);


		//We use .png images here, rather than .svg. They are frequently being created on-the-fly
		//and in large numbers, so maybe this makes a performance difference. Also, there seems
		//to be a bug with Qt's rendering of dialog-information.svg which shows up clearly
		//against the dark background of the log. Using a .png works around this.
		debugIcon = QIcon(QPixmap(QString::fromUtf8(":/images/icons/script-error.png")));
		infoIcon = QIcon(QPixmap(QString::fromUtf8(":/images/icons/dialog-information.png")));
		warningIcon = QIcon(QPixmap(QString::fromUtf8(":/images/icons/dialog-warning.png")));
		errorIcon = QIcon(QPixmap(QString::fromUtf8(":/images/icons/dialog-error.png")));

		//Currently we hard-code the colours for the various log levels
		bgColor.setRgb(0, 0, 0);
		debugColor.setRgb(255, 255, 255);
		infoColor.setRgb(64, 64, 255);
		warningColor.setRgb(255, 255, 64);
		errorColor.setRgb(255,64,64);

		//Set the widgets background to the colour we chose above.
		QPalette palette = m_pLogTable->palette();
		palette.setColor(QPalette::Active, QPalette::Base, bgColor);
		palette.setColor(QPalette::Inactive, QPalette::Base, bgColor);
		m_pLogTable->setPalette(palette);

		//Initial set up of which log levels are displayed
		computeVisibleMessageTypes(true);

		//Create a file to write this log to disk.
		QString filename;
		QTextStream(&filename) << "logs/" << mName << ".html";
		mFile = new QFile(filename, this);
		mFile->open(QFile::WriteOnly | QFile::Truncate | QFile::Text | QIODevice::Unbuffered);
		mTextStream.setDevice(mFile);

		//Write the opening HTML to the log file.
		writeHTMLHeader();
	}

	Log::~Log()
	{
		//Write the closing HTML to the log file.
		writeHTMLTail();
	}

	//Used by the model for determining the decoration to use for an entry
	QIcon &Log::getIcon(LogLevel level)
	{
		switch (level)
		{
			case LL_DEBUG:
				return debugIcon;
			case LL_INFO:
				return infoIcon;
			case LL_WARNING:
				return warningIcon;
			case LL_ERROR:
				return errorIcon;
			default:
				return errorIcon;
		}
	}

	//Used by the model for determining the foreground color to use for an entry
	QColor &Log::getForegroundColour(LogLevel level)
	{
		switch (level)
		{
			case LL_DEBUG:
				return debugColor;
			case LL_INFO:
				return infoColor;
			case LL_WARNING:
				return warningColor;
			case LL_ERROR:
				return errorColor;
			default:
				return debugColor;
		}
	}

	void Log::logMessage(const QString& message, LogLevel logLevel)
	{
		emit _logMessageReceived(message, logLevel);
	}

	void Log::logMessageImpl(const QString& message, LogLevel logLevel)
	{
		LogEntry logEntry(message, logLevel);
		writeMessageToHTML(logEntry);

		mLogModel->append(logEntry);
		m_pLogTable->verticalHeader()->resizeSection(mLogModel->rowCount() - 1, 14);
		if(!m_bSliderPressed)
		{
			m_pLogTable->scrollToBottom();
		}
		if(mForceProcessEvents)
		{
			qApp->processEvents();
		}
	}

	void Log::computeVisibleMessageTypes(bool ignored)
	{
		mVisibleMessageTypes = 0;
		if(showDebugButton->isChecked()) { mVisibleMessageTypes |= LL_DEBUG; }
		if(showInformationButton->isChecked()) { mVisibleMessageTypes |= LL_INFO; }
		if(showWarningsButton->isChecked()) { mVisibleMessageTypes |= LL_WARNING; }
		if(showErrorsButton->isChecked()) { mVisibleMessageTypes |= LL_ERROR; }
		mProxyModel->setVisisbleLevels(mVisibleMessageTypes);
	}

	void Log::setForceProcessEvents(bool forceProcessEvents)
	{
		mForceProcessEvents = forceProcessEvents;
	}

	void Log::writeHTMLHeader(void)
	{
		mTextStream
			<< "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << endl
			<< "<html>" << endl
			<< "<head>" << endl
			<< "<meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">" << endl
			<< "<title>Log File</title>" << endl
			<< "</head>" << endl
			<< "<body style=\"background-color: black; color: rgb(0, 0, 0);\">" << endl
			<< "<table style=\"text-align: left; width: 100%;\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">" << endl
			<< "<tbody>" << endl;
	}

	void Log::writeMessageToHTML(LogEntry entry) 
	{
		QString colour;
		QString icon;
		switch(entry.getLevel())
		{
		case LL_DEBUG:
			colour = "white";
			icon = "script-error.png";
			break;
		case LL_INFO:
			colour = "blue";
			icon = "dialog-information.png";
			break;
		case LL_WARNING:
			colour = "yellow";
			icon = "dialog-warning.png";
			break;
		case LL_ERROR:
			colour = "red";
			icon = "dialog-error.png";
			break;
		}

		mTextStream 
			<< "<tr>"
			<< "<td style=\"text-align: center; width: 25px;\">"
			<< "<img src=\"images/" << icon << "\">"
			<< "</td>"
			<< "<td style=\"width: 90px;\"><span style=\"color: " << colour << ";\">"
			<< entry.getTimestamp().toString("hh:mm:ss a") << " - "
			<< "</span></td>"
			<< "<td><span style=\"color: " << colour << ";\">"
			<< entry.getMessage()
			<< "</span></td></tr>" << endl;

	}

	void Log::writeHTMLTail(void)
	{
		mTextStream
			<< "</tbody>" << endl
			<< "</table>" << endl
			<< "</body>" << endl
			<< "</html>" << endl;
	}

	void Log::onSliderPressed(void)
	{
		m_bSliderPressed = true;
	}

	void Log::onSliderReleased(void)
	{
		m_bSliderPressed = false;
	}
}