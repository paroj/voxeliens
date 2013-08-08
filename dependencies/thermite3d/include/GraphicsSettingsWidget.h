#ifndef THERMITE_GRAPHICSSETTINGSWIDGET_H_
#define THERMITE_GRAPHICSSETTINGSWIDGET_H_

#include "ui_GraphicsSettingsWidget.h"

#include "AbstractSettingsWidget.h"

namespace Thermite
{

	class GraphicsSettingsWidget : public AbstractSettingsWidget, private Ui::GraphicsSettingsWidget
	{
		Q_OBJECT

	public:
		GraphicsSettingsWidget(QWidget *parent = 0);

		void disableFirstTimeOnlySettings(void);
		void readFromSettings(void);
		void writeToSettings(void);

	public slots:

		void on_mDirect3D9RadioButton_toggled(bool checked);
	};
}

#endif /*THERMITE_GRAPHICSSETTINGSWIDGET_H_*/
