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

#include "MusicPlayer.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>

#include "TankWarsApplication.h"

MusicPlayer::MusicPlayer(QObject* parent)
	:QObject(parent),
	music(nullptr)
{

	connect(&mFadeMusicTimer, SIGNAL(timeout()), this, SLOT(fadeMusic()));
}

MusicPlayer::~MusicPlayer()
{
	stopSong();
}

void MusicPlayer::playSong(const QString& name)
{
	mFadeMusicTimer.stop(); //Make sure we are not still fading
	stopSong();
	
	QString fullPath = mDirectory + name;
	music = Mix_LoadMUS(fullPath.toLatin1().data());
	Mix_PlayMusic(music, 0);
	Mix_HookMusicFinished(NULL);
}

void MusicPlayer::fadeOffSong()
{
	if(music)
	{
		mFadeMusicTimer.setInterval(100);
		mFadeMusicTimer.start();
	}
}

void MusicPlayer::stopSong()
{
	if(music)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = nullptr;
	}
}

void MusicPlayer::setDirectory(const QString& directory)
{
	mDirectory = directory;
}

void MusicPlayer::setVolume(qreal newVolume)
{
	if(music)
	{
		Mix_VolumeMusic(newVolume*255.0);
	}
}

qreal MusicPlayer::volume()
{
	if(music)
	{
		return Mix_VolumeMusic(-1)/255.0;
	}
	return 0.0f;
}

void MusicPlayer::fadeMusic(void)
{
	Mix_FadeOutMusic(100);
}
