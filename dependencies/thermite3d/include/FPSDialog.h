#ifndef THERMITE_FPSDIALOG_H_
#define THERMITE_FPSDIALOG_H_

#include "ui_FPSDialog.h"

#include <QTime>
#include <QTimer>

namespace Thermite
{
	class FPSDialog : public QDialog, private Ui::FPSDialog
	{
		Q_OBJECT

	public:
		FPSDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	private slots:
		void updateLCDDisplay(void);

	private:
		QTimer mTimer;
		QTime mTime;

		QPoint dragPosition;

		unsigned int mLastFrameCountValue;
	};
}

#endif /*THERMITE_FPSWIDGET_H_*/
