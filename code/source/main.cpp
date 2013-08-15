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

#include "TankWarsApplication.h"
#include "TankWarsViewWidget.h"

#include <QDesktopWidget>
#include <QFile>
#include <QPushButton>
#include <QIcon>
#include <QDir>
#include <QScopedPointer>

using namespace Thermite;

QScopedPointer<QFile> gLogFile;

void myMessageOutput(QtMsgType type, const char *msg)
{
	QTextStream stream(gLogFile.data());

	switch (type)
	{
	case QtDebugMsg:		
		stream << "Debug: " << msg << "\n";
		break;
	case QtWarningMsg:
		stream << "Warning: " << msg << "\n";
		break;
	case QtCriticalMsg:
		stream << "Critical: " << msg << "\n";
		break;
	case QtFatalMsg:
		stream << "Fatal: " << msg << "\n";
		stream.flush(); //As we are about to abort
		abort();
	}

	stream.flush();
}

int main(int argc, char *argv[])
{
	QString logFileName = "Voxeliens.log";
	QString logFileDirectory;
	
	#ifdef  _WIN32
	logFileDirectory = QCoreApplication::applicationDirPath();
	#else
	// http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	if(!QString(std::getenv("XDG_CACHE_HOME")).isEmpty())
	{
		logFileDirectory = QString(std::getenv("XDG_CACHE_HOME"))+"/VolumesOfFun/Voxeliens/";
	}
	else
	{
		logFileDirectory = QDir::homePath()+"/.cache/VolumesOfFun/Voxeliens/";
	}
	#endif
	
	gLogFile.reset(new QFile(logFileDirectory+logFileName));
	QDir temp;
	temp.mkpath(logFileDirectory);
	
	if(gLogFile->open(QFile::WriteOnly | QFile::Text))
	{
		qInstallMsgHandler(myMessageOutput);
	}

	QString appName = "Voxeliens v1.00";

	qDebug() << "======================================== Starting " << appName << " ========================================";

	QCoreApplication::addLibraryPath(".");

	qDebug() << "Creating application object...";
	TankWarsApplication app(argc, argv);

	qDebug() << "Creating main widget...";
	TankWarsViewWidget mOgreWidget(0, 0);
	mOgreWidget.setWindowTitle(appName);

	qDebug() << "Initialising Ogre...";
	Ogre::NameValuePairList ogreWindowParams;
	
	mOgreWidget.initialiseOgre(&ogreWindowParams);

	mOgreWidget.initialise();

	int width = mOgreWidget.mVoxeliensSettings->value("graphics/width", 1024).toInt();
	int height = mOgreWidget.mVoxeliensSettings->value("graphics/height", 768).toInt();

	QRect parentGeometry = app.desktop()->availableGeometry();
	int defaultXPos = (parentGeometry.width() - width) / 2;
	int defaultYPos = (parentGeometry.height() - height) / 2;

	int xPos = mOgreWidget.mVoxeliensSettings->value("graphics/xpos", defaultXPos).toInt();
	int yPos = mOgreWidget.mVoxeliensSettings->value("graphics/ypos", defaultYPos).toInt();

	mOgreWidget.resize(width,height);
	mOgreWidget.move(xPos, yPos);

	//app.centerWidget(&mOgreWidget);
	app.mViewWidgets.append(&mOgreWidget);

	bool maximized = mOgreWidget.mVoxeliensSettings->value("graphics/isMaximized", false).toBool();
	if(maximized)
	{
		mOgreWidget.showMaximized();
	}
	else
	{
		mOgreWidget.show();
	}

	mOgreWidget.widthBeforeFullscreen = mOgreWidget.width();
	mOgreWidget.heightBeforeFullscreen = mOgreWidget.height();
	mOgreWidget.xPosBeforeFullscreen = mOgreWidget.frameGeometry().left();
	mOgreWidget.yPosBeforeFullscreen = mOgreWidget.frameGeometry().top();
	bool runFullscreen = mOgreWidget.mVoxeliensSettings->value("graphics/runFullscreen", false).toBool();
	if(runFullscreen)
	{
		mOgreWidget.changeWindowSetup(true);
	}

	qDebug() << "Starting main loop...";
	return app.exec();
}