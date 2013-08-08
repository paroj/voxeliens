#ifndef THERMITE_SETTINGSDIALOG_H_
#define THERMITE_SETTINGSDIALOG_H_

#include "ui_SettingsDialog.h"

#include "AbstractSettingsWidget.h"

namespace Thermite
{

	class SettingsDialog : public QDialog, private Ui::SettingsDialog
	{
		Q_OBJECT

	public:
		SettingsDialog(QSettings* settings, QWidget *parent = 0);

		void addSettingsWidget(const QString& title, AbstractSettingsWidget* settingsWidget);

		QSettings* mSettings;
	};
}

#endif /*THERMITE_SETTINGSDIALOG_H_*/
