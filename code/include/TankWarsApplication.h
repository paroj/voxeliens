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

#ifndef TANKWARS_APPLICATION_H_
#define TANKWARS_APPLICATION_H_

#include "Application.h"

#include "MusicPlayer.h"

using namespace Thermite;

class TankWarsApplication : public Application
{
public:
	TankWarsApplication(int & argc, char ** argv);
	~TankWarsApplication();

	//void update(void);
	void shutdown(void);

	MusicPlayer* mMusicPlayer;
	
	QString mResourcePath;

private:
	TankWarsViewWidget* mOgreWidget;
};

//This redefines qApp, causing it to return an TankWarsApplication pointer instead of a QApplication one.
//This is useful, for example, to access the logging system. This is done in the same way that
//Qt does it to get a QApplication rather than a QCoreApplication, so it should be legitimate.
#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<TankWarsApplication *>(QCoreApplication::instance()))

#endif //TANKWARS_APPLICATION_H_