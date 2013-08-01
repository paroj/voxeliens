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

#ifndef KEYBINDINGDIALOG_H_
#define KEYBINDINGDIALOG_H_

#include "ui_KeyBindingDialog.h"

#include <QSettings>

class KeyBindingDialog : public QDialog, private Ui::KeyBindingDialog
{
	Q_OBJECT

public:
	KeyBindingDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	void keyPressEvent(QKeyEvent* event);

	quint32 mSelectedNativeVirtualKey;
};

#endif /*KEYBINDINGDIALOG_H_*/
