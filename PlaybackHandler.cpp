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


#include "PlaybackHandler.h"

#include "AnsiFile.h"
#include "FileType.h"

#include "ModPlayer.h"
#include "DigiBoosterPlayer.h"
#include "DigiBoosterProPlayer.h"
#include "SymphoniePlayer.h"
#include "TfmxPlayer.h"
#include "AhxPlayer.h"
#include "OktalyzerPlayer.h"
#include "ProTrackerPlayer.h"
#include "ScreamTrackerPlayer.h"
#include "ImpulseTrackerPlayer.h"
#include "MadTracker2Player.h"


PlaybackHandler::PlaybackHandler(QObject *parent) :
    QObject(parent),
    m_pFileBuffer(nullptr),
    m_pModPlayer(nullptr),
    m_pDecodeBuffer(nullptr),
	m_pAudioOut(nullptr)
{
    // we can adjust size later,
    // also reusable with suffcient buffer size
    m_pDecodeBuffer = new CReadBuffer();
    m_pFileBuffer = new CReadBuffer();
}

PlaybackHandler::~PlaybackHandler()
{
    // TODO: deleted automatically by output-device?
	m_pDevOut = nullptr;
    
    if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
	}

    if (m_pModPlayer != nullptr)
    {
        delete m_pModPlayer;
        m_pModPlayer = nullptr;
    }
    delete m_pFileBuffer;
    delete m_pDecodeBuffer;
}

void PlaybackHandler::playFile(QString filename)
{
    if (m_pAudioOut != nullptr)
    {
        stopPlay();
    }
    
    PlayFile(filename);
}

void PlaybackHandler::stopPlay()
{
    // TODO: deleted automatically by output-device?
	m_pDevOut = nullptr;

	if (m_pAudioOut != nullptr)
	{
		m_pAudioOut->stop();
		delete m_pAudioOut;
		m_pAudioOut = nullptr;
	}
    
    if (m_pModPlayer != nullptr)
    {
        delete m_pModPlayer;
        m_pModPlayer = nullptr;
    }
}


void PlaybackHandler::onAudioState(QAudio::State enState)
{
    // state change handling (play started/stopped)
    if (enState == QAudio::ActiveState)
	{
        // show that we are playing
	}
	else if (enState == QAudio::StoppedState)
	{
		// show that we stopped
        // cleanup
	}
}

void PlaybackHandler::onPlayNotify()
{
    // trigger in certaing intervals from audiodevice
    // to write into buffer more data so no gaps occur

    /*    
    qint64 nWritten = m_pDevOut->write(m_pSampleData, m_nSampleDataSize);
	if (nWritten == -1)
	{
		on_actionStop_triggered();
		return;
	}
    */
}

CModPlayer *PlaybackHandler::GetPlayer(CReadBuffer *fileBuffer) const
{
    if (fileBuffer->GetSize() < 32)
    {
        // nothing useful in the file..
        return nullptr;
    }
    
    CModPlayer *pModPlayer = nullptr;
    CFileType type;
    
    // some types have identifier somewhere middle of file..
    // just tell what we have for identifying it.
    type.DetermineFileType(fileBuffer->GetBegin(), fileBuffer->GetSize());
    
    switch (type.m_enFileType)
    {
    //case HEADERTYPE_MOD_SOUNDTRACKER: // original, can we detect?
    case HEADERTYPE_MOD_PROTRACKER:
    case HEADERTYPE_MOD_NOISETRACKER:
    case HEADERTYPE_MOD_STARTREKKER:
        // ? maybe share same player for these, check differences
        pModPlayer = new CProTrackerPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_OCTAMED:
        //pModPlayer = new COctaMedPlayer(fileBuffer);
        break;
    case HEADERTYPE_OCTAMED_OSS:
        //pModPlayer = new COctaMedOSSPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_SYMMOD:
        pModPlayer = new CSymphoniePlayer(fileBuffer);
        break;

    case HEADERTYPE_DIGIBOOSTER:
        pModPlayer = new CDigiBoosterPlayer(fileBuffer);
        break;
    case HEADERTYPE_DBMPRO:
        pModPlayer = new CDigiBoosterProPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_AHX:
        pModPlayer = new CAhxPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_TFMX:
        pModPlayer = new CTfmxPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_OKTALYZER:
        pModPlayer = new COktalyzerPlayer(fileBuffer);
        break;
    
    case HEADERTYPE_IT:
        pModPlayer = new CImpulseTrackerPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_S3M:
        pModPlayer = new CScreamTrackerPlayer(fileBuffer);
        break;
        
    case HEADERTYPE_MADTRACKER2:
        pModPlayer = new CMadTracker2Player(fileBuffer);
        break;

    default:
        // just to silence GCC..
        break;
    }
    return pModPlayer;
}

void PlaybackHandler::PlayFile(QString &filename)
{
    // TODO: different thread for module handling?

    // note: change to memory-mapped files later..
    CAnsiFile file(filename.toStdString());
    if (file.IsOk() == false)
    {
        emit error("Failed to open file: " + filename);
        return;
    }

    // clear&reuse or allocate new if necessary
    m_pFileBuffer->PrepareBuffer(file.GetSize(), false);
    if (file.Read(m_pFileBuffer->GetBegin(), file.GetSize()) == false)
    {
        emit error("Failed to read file: " + filename);
        return;
    }
    file.Close(); // can close already

    // keep as member also..
    m_pModPlayer = GetPlayer(m_pFileBuffer);
    if (m_pModPlayer == nullptr)
    {
        emit error("Failed to create player");
        return;
    }
    
    if (m_pModPlayer->ParseFileInfo() == false)
    {
        emit error("File could not be handled");
        return;
    }
    
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (info.isNull() == true)
    {
        emit error("Failed to get default audio output");
		return;
    }
    
    // TODO: get info on what is suitable format for playing..
    // (what is supported by player/format/module..)
    //QAudioFormat format = pModPlayer->GetOutputFormat();

    /*
    // placeholder..
    QAudioFormat format;
    m_pAudioOut = new QAudioOutput(format, this);
    
    // TODO: change&notify handlers (push new data when enough has played)
	//connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
	//connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));
    
    // count size of buffer for decoding:
    // channels, samplesize, frequency etc.
    // just guess for now: 
    // expect 2-channels and 16-bit samples at 44.1kHz,
    // get proper values later
    int64_t nBuffer = ( 2 * 2 * 44100);

    int iBufSize = m_pAudioOut->bufferSize();
	if (iBufSize < nBuffer)
	{
		m_pAudioOut->setBufferSize(nBuffer);
	}
    m_pAudioOut->setNotifyInterval(250); // 250ms
    
    // create buffer for decoding 
    //QByteArray decodeBuffer();
    //CReadBuffer decodeBuffer(nBuffer);
    m_pDecodeBuffer->PrepareBuffer(nBuffer, false);
    
    // placeholder..
    //size_t nInBuf = m_pModPlayer->Decode(m_pDecodeBuffer->GetBegin(), m_pDecodeBuffer->GetSize());
    //qint64 nWritten = m_pDevOut->write(m_pDecodeBuffer->GetBegin(), nInBuf);
    */
}
