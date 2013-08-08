#ifndef THERMITE_ABSTRACTSETTINGSWIDGET_H_
#define THERMITE_ABSTRACTSETTINGSWIDGET_H_

#include <QWidget>

class QSettings;

namespace Thermite
{
	class AbstractSettingsWidget : public QWidget
	{
		Q_OBJECT

	public:
		AbstractSettingsWidget(QWidget *parent = 0);

		void setSettings(QSettings* settings);

		///Reimplement this function and within it disable any elements
		///which cannot be changed once the application has started
		virtual void disableFirstTimeOnlySettings(void) = 0;

	public slots:
		void dialogFinished(int result);
		virtual void readFromSettings(void) = 0;
		virtual void writeToSettings(void) = 0;


	protected:
		QSettings* mSettings;
	};
}

#endif /*THERMITE_ABSTRACTSETTINGSWIDGET_H_*/
