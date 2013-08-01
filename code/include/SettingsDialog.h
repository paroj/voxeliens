/*******************************************************************************
Copyright (c) 2012-2013 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*******************************************************************************/

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include "ui_SettingsDialog.h"

#include <QSettings>

quint32 defaultForwardVirtualKey(void);
quint32 defaultBackVirtualKey(void);
quint32 defaultLeftVirtualKey(void);
quint32 defaultRightVirtualKey(void);
quint32 defaultFireVirtualKey(void);

namespace Difficulties
{
	enum Difficulty
	{
		Easy,
		Normal,
		Hard
	};
}
typedef Difficulties::Difficulty Difficulty;

class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
	Q_OBJECT

public:
	SettingsDialog(QWidget *parent = 0);

	void readFromSettings(QSettings* settings);
	void writeToSettings(QSettings* settings);

public slots:
	void on_mMoveForwardButton_clicked(bool checked);
	void on_mMoveBackButton_clicked(bool checked);
	void on_mMoveLeftButton_clicked(bool checked);
	void on_mMoveRightButton_clicked(bool checked);
	void on_mFireButton_clicked(bool checked);

private:
	quint32 mMoveForwardNativeVirtualKey;
	quint32 mMoveBackNativeVirtualKey;
	quint32 mMoveLeftNativeVirtualKey;
	quint32 mMoveRightNativeVirtualKey;
	quint32 mFireNativeVirtualKey;
};

#endif /*SETTINGSDIALOG_H_*/
