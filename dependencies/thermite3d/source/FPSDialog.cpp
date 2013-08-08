#include "FPSDialog.h"

#include "Application.h"

#include <QMouseEvent>

namespace Thermite
{
	FPSDialog::FPSDialog(QWidget* parent, Qt::WindowFlags f)
	:QDialog(parent, f)
	,mLastFrameCountValue(0)
	{
		setupUi(this);

		connect(&mTimer, SIGNAL(timeout(void)), this, SLOT(updateLCDDisplay(void)));

		mTimer.start(60000);
		mTime.restart();
	}

	void FPSDialog::mousePressEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::LeftButton)
		{
			dragPosition = event->globalPos() - frameGeometry().topLeft();
			event->accept();
		}
	} 

	void FPSDialog::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			QPoint desiredPos = event->globalPos() - dragPosition;
			QRect desiredRect(desiredPos, size());
			if(parentWidget()->geometry().contains(desiredRect, true))
			{
				move(desiredPos);
			}
			event->accept();
		}
	} 

	void FPSDialog::updateLCDDisplay(void)
	{		
		unsigned int currentFrameCountValue = qApp->frameCount();
		unsigned int framesRendered = currentFrameCountValue - mLastFrameCountValue;

		//In theory we know the time elapsed, because a timer is calling this function at
		//fixed intervals. We don't know how accurate this is though, so it seems sensible
		//to get the actual time elapsed.
		float timeElapsedInSeconds = mTime.elapsed() / 1000.0f;

		qDebug() << "FPS = " << framesRendered / timeElapsedInSeconds;

		mLcdNumber->display(static_cast<int>(framesRendered / timeElapsedInSeconds));

		//Reset ready for next time.
		mLastFrameCountValue = currentFrameCountValue;
		mTime.restart();
	}
}
