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
	:QObject(parent)
	,mMediaObject(0)
{
	mMediaObject = new Phonon::MediaObject(this);
	mAudioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	mPath = Phonon::createPath(mMediaObject, mAudioOutput);

	connect(&mFadeMusicTimer, SIGNAL(timeout()), this, SLOT(fadeMusic()));
}

void MusicPlayer::playSong(const QString& name)
{
	if(mMediaObject && mAudioOutput)
	{
		mFadeMusicTimer.stop(); //Make sure we are not still fading
		stopSong();

		QString fullPath = mDirectory + name;
		mMediaObject->setCurrentSource(Phonon::MediaSource(fullPath));
		setVolume(1.0);
		mMediaObject->play();
	}
}

void MusicPlayer::fadeOffSong(void)
{
	if(mMediaObject && mAudioOutput)
	{
		mFadeMusicTimer.setInterval(100);
		mFadeMusicTimer.start();
	}
}

void MusicPlayer::stopSong(void)
{
	if(mMediaObject && mAudioOutput)
	{
		mMediaObject->stop();
	}
}

void MusicPlayer::setDirectory(const QString& directory)
{
	mDirectory = directory;
}

void MusicPlayer::setVolume(qreal newVolume)
{
	if(mMediaObject && mAudioOutput)
	{
		mAudioOutput->setVolume(newVolume);
	}
}

qreal MusicPlayer::volume(void)
{
	if(mMediaObject && mAudioOutput)
	{
		return mAudioOutput->volume();
	}
	return 0.0f;
}

void MusicPlayer::fadeMusic(void)
{
	if(mMediaObject && mAudioOutput)
	{
		qreal volume = qApp->mMusicPlayer->volume();
		if(volume > 0.01)
		{
			volume -= 0.05;
			qApp->mMusicPlayer->setVolume(volume);
		}
		else
		{
			mFadeMusicTimer.stop();
		}
	}
}
