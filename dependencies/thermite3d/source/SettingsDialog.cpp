#include "SettingsDialog.h"

namespace Thermite
{
	SettingsDialog::SettingsDialog(QSettings* settings, QWidget *parent)
	:QDialog(parent)
	,mSettings(settings)
	{
		setupUi(this);
	}

	void SettingsDialog::addSettingsWidget(const QString& title, AbstractSettingsWidget* settingsWidget)
	{
		settingsWidget->setSettings(mSettings);
		settingsWidget->readFromSettings();
		mTabWidget->addTab(settingsWidget, title);

		//NOTE: We need to make sure the settings are copied from the dialogs widgets into the settings
		//object before the main application tries to retrieve them. The main app does this on the 
		//accepted() signal, but because signal order is not guarenteed we cannot use this for copying the
		//settings as well. Looking st the Qt source shows that the finished signal is emitted first,
		//though the documentation doesn't state this.
		connect(this, SIGNAL(finished(int)), settingsWidget, SLOT(dialogFinished(int)));
	}
}
