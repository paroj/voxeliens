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

#include <QDir>

#include <phonon/mediaobject.h>

TankWarsApplication::TankWarsApplication(int & argc, char ** argv)
	:Application(argc, argv)
	,mOgreWidget(0)
{
	//mResourcePath = "../../resources/";
	mResourcePath = "./";
	QDir pathToResources(mResourcePath);
	if(!pathToResources.exists())
	{
		QString message("Path to resources does not exist: " + pathToResources.path());
		qApp->showErrorMessageBox(message);
	}

	//Initialise random number generator
	qsrand(QDateTime::currentDateTime().toTime_t());

	//Initialise all resources		
	//addResourceDirectory("./resources/");
	addResourceDirectory(pathToResources.path());

	mMusicPlayer = new MusicPlayer(this);
	//mMusicPlayer->setDirectory("./audio/music/");
	mMusicPlayer->setDirectory(mResourcePath+"audio/music/");
	//mMusicPlayer->playRandomSong();
}

TankWarsApplication::~TankWarsApplication()
{
	if(mOgreWidget)
	{
		delete mOgreWidget;
		mOgreWidget = 0;
	}
}

/*void TankWarsApplication::update(void)
{
	if(mOgreWidget)
	{
		mOgreWidget->update();
	}

	Application::update();
}*/

void TankWarsApplication::shutdown(void)
{
	mOgreWidget->shutdown();
}
