//////////////////////////////////////
//
// PlaybackHandler:
// move module and playback handling
// from main window to separate class,
// this will simplify later when 
// GUI and playback are separated..
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef PLAYBACKHANDLER_H
#define PLAYBACKHANDLER_H

#include <QObject>
#include <QtMultimedia/QAudioOutput>
#include <QFile>

#include "ArchiveHandler.h"

class CModPlayer;
//class CFileType;
class CReadBuffer;
class DecodeCtx;

class PlaybackHandler : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackHandler(QObject *parent = 0);
    virtual ~PlaybackHandler();

signals:
    void playbackFinished();
	void status(QString message);
    void error(QString message);

public slots:
    void playFile(QString filename);
    void stopPlay();

private slots:
    void onAudioState(QAudio::State enState);
	void onPlayNotify();
    
protected:
	bool initialOutput();
    CModPlayer *GetPlayer(CReadBuffer *fileBuffer) const;
    void PlayFile(QString &filename);
    
private:
	// compressed file support
	// (LhA, LZX, XPK, PowerPacker etc.)
	ArchiveHandler *m_pArchiveHandler;

    QFile *m_pFile;
	QString m_currentFilename;
    
    CReadBuffer *m_pFileBuffer;
    CModPlayer *m_pModPlayer;
    
    CReadBuffer *m_pDecodeBuffer;
	DecodeCtx *m_pCtx;

    QAudioOutput *m_pAudioOut;
	QIODevice *m_pDevOut;
	
	size_t m_nInBuf; // amount waiting in buffer
	qint64 m_Written; // amount written to output device buffer
};

#endif // PLAYBACKHANDLER_H
