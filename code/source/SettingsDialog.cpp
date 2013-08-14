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

#include "SettingsDialog.h"

#include "Application.h"
#include "KeyBindingDialog.h"

#include <QKeyEvent>

#ifdef 	_WIN32
#include <Windows.h>
#else
#include <QX11Info>
#include <X11/XKBlib.h>
#endif

//From here: http://altdevblogaday.com/2011/10/02/i-never-managed-to-go-left-on-first-try/
//These are called 'Keycodes' on X11
#ifdef  _WIN32
const quint32 ScanCodeW = 17; //TODO are these correct for Linux?
const quint32 ScanCodeA = 30;
const quint32 ScanCodeS = 31;
const quint32 ScanCodeD = 32;
const quint32 ScanCodeSpace = 57;
#else
const quint32 ScanCodeW = 25; //TODO are these correct for Linux?
const quint32 ScanCodeA = 38;
const quint32 ScanCodeS = 39;
const quint32 ScanCodeD = 40;
const quint32 ScanCodeSpace = 65;
#endif

QString GetKeyName(unsigned int virtualKey)
{
#ifdef  _WIN32
	unsigned int scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	// because MapVirtualKey strips the extended bit for some keys
	switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
	case VK_PRIOR: case VK_NEXT: // page up and page down
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE: // numpad slash
	case VK_NUMLOCK:
		{
			scanCode |= 0x100; // set extended bit
			break;
		}
	}

	char keyName[256];
	QString name;

	if (GetKeyNameTextA(scanCode << 16, keyName, 256) != 0)
	{
		name = keyName;
	}
#else
	QString name = XKeysymToString(virtualKey);
#endif
	return name;
}


int mapScanCodeToVirtualKey(int scanCode)
{
#ifdef  _WIN32
	int nativeVirtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);
#else
	int nativeVirtualKey = XkbKeycodeToKeysym(QX11Info::display(), scanCode, 0, 0);
#endif	
	return nativeVirtualKey;
}

quint32 defaultForwardVirtualKey(void)
{
	 return mapScanCodeToVirtualKey(ScanCodeW);
}

quint32 defaultBackVirtualKey(void)
{
	 return mapScanCodeToVirtualKey(ScanCodeS);
}

quint32 defaultLeftVirtualKey(void)
{
	 return mapScanCodeToVirtualKey(ScanCodeA);
}

quint32 defaultRightVirtualKey(void)
{
	 return mapScanCodeToVirtualKey(ScanCodeD);
}

quint32 defaultFireVirtualKey(void)
{
	 return mapScanCodeToVirtualKey(ScanCodeSpace);
}

SettingsDialog::SettingsDialog(QWidget *parent)
	:QDialog(parent)
{
	setupUi(this);	
}

void SettingsDialog::readFromSettings(QSettings* settings)
{
	bool runFullscreen = settings->value("graphics/runFullscreen", false).toBool();
	mWindowedRadioButton->setChecked(!runFullscreen);
	mFullscreenRadioButton->setChecked(runFullscreen);

	mMoveForwardNativeVirtualKey = settings->value("input/forwardKey", defaultForwardVirtualKey()).toUInt();
	mMoveForwardButton->setText(QString("PRESS '") + GetKeyName(mMoveForwardNativeVirtualKey) + QString("'"));

	mMoveBackNativeVirtualKey = settings->value("input/backKey", defaultBackVirtualKey()).toUInt();
	mMoveBackButton->setText(QString("PRESS '") + GetKeyName(mMoveBackNativeVirtualKey) + QString("'"));

	mMoveLeftNativeVirtualKey = settings->value("input/leftKey", defaultLeftVirtualKey()).toUInt();
	mMoveLeftButton->setText(QString("PRESS '") + GetKeyName(mMoveLeftNativeVirtualKey) + QString("'"));

	mMoveRightNativeVirtualKey = settings->value("input/rightKey", defaultRightVirtualKey()).toUInt();
	mMoveRightButton->setText(QString("PRESS '") + GetKeyName(mMoveRightNativeVirtualKey) + QString("'"));

	mFireNativeVirtualKey = settings->value("input/fireKey", defaultFireVirtualKey()).toUInt();
	mFireButton->setText(QString("CLICK MOUSE OR PRESS '") + GetKeyName(mFireNativeVirtualKey) + QString("'"));

	bool moveRelativeToWorld = settings->value("input/moveRelativeToWorld", false).toBool();
	mWorldRadioButton->setChecked(moveRelativeToWorld);
	mCameraRadioButton->setChecked(!moveRelativeToWorld);

	float mouseSensitivity = settings->value("input/mouseSensitivity", 0.1f).toFloat();
	int mouseSensitivityAsInt = static_cast<int>(mouseSensitivity * 100.0 + 0.0001); //Because the slider only handles int positions
	mMouseSensitivitySlider->setValue(mouseSensitivityAsInt);

	int difficulty = settings->value("difficulty", Difficulties::Normal).toUInt();
	switch(difficulty)
	{
	case Difficulties::Easy:
		{
			mEasyRadioButton->setChecked(true);
			break;
		}
	case Difficulties::Normal:
		{
			mNormalRadioButton->setChecked(true);
			break;
		}
	case Difficulties::Hard:
		{
			mHardRadioButton->setChecked(true);
			break;
		}
	}
}

void SettingsDialog::writeToSettings(QSettings* settings)
{
	settings->setValue("graphics/runFullscreen", mFullscreenRadioButton->isChecked());

	settings->setValue("input/forwardKey", mMoveForwardNativeVirtualKey);
	settings->setValue("input/backKey", mMoveBackNativeVirtualKey);
	settings->setValue("input/leftKey", mMoveLeftNativeVirtualKey);
	settings->setValue("input/rightKey", mMoveRightNativeVirtualKey);
	settings->setValue("input/fireKey", mFireNativeVirtualKey);

	settings->setValue("input/moveRelativeToWorld", mWorldRadioButton->isChecked());

	qDebug() << "Input relative to world = " << mWorldRadioButton->isChecked();

	float mouseSensitivity = mMouseSensitivitySlider->value();
	mouseSensitivity /= 100.0f; //Because the slider only handles int positions
	settings->setValue("input/mouseSensitivity", QString::number(mouseSensitivity));

	if(mEasyRadioButton->isChecked())
		settings->setValue("difficulty", static_cast<quint32>(Difficulties::Easy));
	if(mNormalRadioButton->isChecked())
		settings->setValue("difficulty", static_cast<quint32>(Difficulties::Normal));
	if(mHardRadioButton->isChecked())
		settings->setValue("difficulty", static_cast<quint32>(Difficulties::Hard));
}

void SettingsDialog::on_mMoveForwardButton_clicked(bool checked)
{
	KeyBindingDialog keyDlg(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	keyDlg.exec();

	mMoveForwardNativeVirtualKey = keyDlg.mSelectedNativeVirtualKey;
	mMoveForwardButton->setText(QString("PRESS '") + GetKeyName(mMoveForwardNativeVirtualKey) + QString("'"));
}

void SettingsDialog::on_mMoveBackButton_clicked(bool checked)
{
	KeyBindingDialog keyDlg(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	keyDlg.exec();

	mMoveBackNativeVirtualKey = keyDlg.mSelectedNativeVirtualKey;
	mMoveBackButton->setText(QString("PRESS '") + GetKeyName(mMoveBackNativeVirtualKey) + QString("'"));
}

void SettingsDialog::on_mMoveLeftButton_clicked(bool checked)
{
	KeyBindingDialog keyDlg(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	keyDlg.exec();

	mMoveLeftNativeVirtualKey = keyDlg.mSelectedNativeVirtualKey;
	mMoveLeftButton->setText(QString("PRESS '") + GetKeyName(mMoveLeftNativeVirtualKey) + QString("'"));
}

void SettingsDialog::on_mMoveRightButton_clicked(bool checked)
{
	KeyBindingDialog keyDlg(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	keyDlg.exec();

	mMoveRightNativeVirtualKey = keyDlg.mSelectedNativeVirtualKey;
	mMoveRightButton->setText(QString("PRESS '") + GetKeyName(mMoveRightNativeVirtualKey) + QString("'"));
}

void SettingsDialog::on_mFireButton_clicked(bool checked)
{
	KeyBindingDialog keyDlg(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	keyDlg.exec();

	mFireNativeVirtualKey = keyDlg.mSelectedNativeVirtualKey;
	mFireButton->setText(QString("CLICK MOUSE OR PRESS '") + GetKeyName(mFireNativeVirtualKey) + QString("'"));
}
