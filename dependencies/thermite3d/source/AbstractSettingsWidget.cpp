#include "AbstractSettingsWidget.h"

#include <QDialog>
#include <QSettings>

namespace Thermite
{
	AbstractSettingsWidget::AbstractSettingsWidget(QWidget *parent)
	:QWidget(parent)
	{
	}

	void AbstractSettingsWidget::setSettings(QSettings* settings)
	{
		mSettings = settings;
	}

	void AbstractSettingsWidget::dialogFinished(int result)
	{
		if(result == QDialog::Accepted)
		{
			writeToSettings();
			disableFirstTimeOnlySettings();
		}
	}
}
