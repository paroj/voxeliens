#include "GraphicsSettingsWidget.h"

#include "Application.h"

#include <QSettings>

namespace Thermite
{
	GraphicsSettingsWidget::GraphicsSettingsWidget(QWidget *parent)
	:AbstractSettingsWidget(parent)
	{
		setupUi(this);	
	}

	void GraphicsSettingsWidget::disableFirstTimeOnlySettings(void)
	{
		mOpenGLRadioButton->setEnabled(false);
		mDirect3D9RadioButton->setEnabled(false);

		mAllowPerfHUDCheckBox->setEnabled(false);
		mEnableGammaCorrectionCheckBox->setEnabled(false);
		mFSSAFactorSpinBox->setEnabled(false);
		mFSSAFactorLabel->setEnabled(false);
		mEnableVerticalSyncCheckBox->setEnabled(false);
	}

	void GraphicsSettingsWidget::readFromSettings(void)
	{
		// ---------- Render System Settings ----------
		mOpenGLRadioButton->setEnabled(/*qApp->isOpenGLAvailable()*/ false);
		mDirect3D9RadioButton->setEnabled(/*qApp->isDirect3D9Available()*/ false);

		QString renderSystem = mSettings->value("Graphics/RenderSystem").toString();
		//Doing OpenGL last here means that is it is the default.
		mDirect3D9RadioButton->setChecked(mDirect3D9RadioButton->isEnabled() && (renderSystem.compare("Direct3D9 Rendering Subsystem") == 0));
		mOpenGLRadioButton->setChecked(mOpenGLRadioButton->isEnabled() && (renderSystem.compare("OpenGL Rendering Subsystem") == 0));		

		// ---------- Window Settings ----------
		QStringList windowModes = mSettings->value("Graphics/WindowModes").toStringList();
		int selectedWindowMode = mSettings->value("Graphics/SelectedWindowMode", 0).toInt();

		if(windowModes.size() > selectedWindowMode) //Make sure it's a valid index.
		{
			mWindowModeComboBox->insertItems(0, windowModes);
			mWindowModeComboBox->setCurrentIndex(selectedWindowMode);
		}

		// ---------- Advanced Settings ----------
		mAllowPerfHUDCheckBox->setChecked(mSettings->value("Graphics/AllowPerfHUD", false).toBool());
		mEnableGammaCorrectionCheckBox->setChecked(mSettings->value("Graphics/EnableGammaCorrection", false).toBool());
		mFSSAFactorSpinBox->setValue(mSettings->value("Graphics/FSSAFactor", 0).toInt());
		mEnableVerticalSyncCheckBox->setChecked(mSettings->value("Graphics/EnableVerticalSync", false).toBool());
	}

	void GraphicsSettingsWidget::writeToSettings(void)
	{
		// ---------- Render System Settings ----------
		if(mOpenGLRadioButton->isChecked())
		{
			mSettings->setValue("Graphics/RenderSystem", "OpenGL Rendering Subsystem");
		}
		else if(mDirect3D9RadioButton->isChecked())
		{
			mSettings->setValue("Graphics/RenderSystem", "Direct3D9 Rendering Subsystem");
		}
		else
		{
			//This can happen if neither render system is available, although we should have been warned earlier?
			Application::showErrorMessageBox("A valid render system has not been selected");
		}

		// ---------- Window Settings ----------
		mSettings->setValue("Graphics/SelectedWindowMode", mWindowModeComboBox->currentIndex());

		//Advanced Settings
		mSettings->setValue("Graphics/AllowPerfHUD", mAllowPerfHUDCheckBox->isChecked());
		mSettings->setValue("Graphics/EnableGammaCorrection", mEnableGammaCorrectionCheckBox->isChecked());
		mSettings->setValue("Graphics/FSSAFactor", mFSSAFactorSpinBox->value());
		mSettings->setValue("Graphics/EnableVerticalSync", mEnableVerticalSyncCheckBox->isChecked());
	}

	void GraphicsSettingsWidget::on_mDirect3D9RadioButton_toggled(bool checked)
	{
		//PerfHUD is only available when using the Direct3D9 render system
		mAllowPerfHUDCheckBox->setEnabled(checked);
		if(!checked)
		{
			mAllowPerfHUDCheckBox->setChecked(false);
		}
	}
}
