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


class CModPlayer;
//class CFileType;
class CReadBuffer;

class PlaybackHandler : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackHandler(QObject *parent = 0);
    virtual ~PlaybackHandler();

signals:
    void playbackFinished();
    void error(QString message);

public slots:
    void playFile(QString filename);
    void stopPlay();

private slots:
    void onAudioState(QAudio::State enState);
	void onPlayNotify();
    
protected:
    CModPlayer *GetPlayer(CReadBuffer *fileBuffer) const;
    void PlayFile(QString &filename);
    
private:
    QFile *m_pFile;
    
    CReadBuffer *m_pFileBuffer;
    CModPlayer *m_pModPlayer;
    
    CReadBuffer *m_pDecodeBuffer;

    QAudioOutput *m_pAudioOut;
	QIODevice *m_pDevOut;
};

#endif // PLAYBACKHANDLER_H
