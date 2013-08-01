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

#ifndef MUSICPLAYER_H_
#define MUSICPLAYER_H_

#include <QList>
#include <QObject>
#include <QTimer>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/path.h>

class MusicPlayer : public QObject
{
	Q_OBJECT

public:
	MusicPlayer(QObject * parent = 0);

	void playSong(const QString& name);
	void stopSong(void);
	void fadeOffSong(void);

	void setDirectory(const QString& directory);
	void setVolume(qreal newVolume);
	qreal volume(void);

private slots:
	void fadeMusic(void);

private:
	QString mDirectory;
	Phonon::MediaObject*	mMediaObject;
	Phonon::AudioOutput*	mAudioOutput;
	Phonon::Path			mPath;

	QTimer mFadeMusicTimer;
};

#endif //MUSICPLAYER_H_
