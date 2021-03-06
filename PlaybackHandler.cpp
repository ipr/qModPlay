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

// archived and compressed files:
// decompression/unpack support
#include "ArchiveHandler.h"

#include "ModPlayer.h"
#include "DigiBoosterPlayer.h"
#include "DigiBoosterProPlayer.h"
#include "SymphoniePlayer.h"
#include "TfmxPlayer.h"
#include "AhxPlayer.h"
#include "OktalyzerPlayer.h"
#include "ProTrackerPlayer.h"
#include "ScreamTrackerPlayer.h"
#include "FastTrackerPlayer.h"
#include "ImpulseTrackerPlayer.h"
#include "MadTracker2Player.h"


PlaybackHandler::PlaybackHandler(QObject *parent) :
    QObject(parent),
    m_pArchiveHandler(nullptr),
    m_pFile(nullptr),
    m_pFileBuffer(nullptr),
    m_pModPlayer(nullptr),
    m_pDecodeBuffer(nullptr),
	m_pAudioOut(nullptr)
{
	// compressed file support
	m_pArchiveHandler = new ArchiveHandler(this);

    // we can adjust size later,
    // also reusable with suffcient buffer size
    //m_pFileBuffer = new CReadBuffer();
    m_pDecodeBuffer = new CReadBuffer();
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
    delete m_pDecodeBuffer;
    delete m_pFileBuffer;
    delete m_pFile;
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
	
	//emit status("Playback stopped");
}


void PlaybackHandler::onAudioState(QAudio::State enState)
{
    // state change handling (play started/stopped)
    if (enState == QAudio::ActiveState)
	{
        // show that we are playing
		emit status("Playing: " + m_currentFilename);
	}
	else if (enState == QAudio::StoppedState)
	{
		// show that we stopped
        // cleanup
		emit status("Stopped");
	}
}

void PlaybackHandler::onPlayNotify()
{
    // trigger in certaing intervals from audiodevice
    // to write into buffer more data so no gaps occur

	// continue more into buffer..
	// TODO: also check how much actually was written before overwriting..
	//
	m_nInBuf += m_pModPlayer->Decode(m_pDecodeBuffer->GetBegin() + m_nInBuf, m_pDecodeBuffer->GetSize() - m_nInBuf);
	
    qint64 nWritten = m_pDevOut->write(m_pDecodeBuffer->GetBegin(), m_nInBuf);
	if (nWritten == -1)
	{
		stopPlay();
		return;
	}
	m_Written += nWritten;

	// if all did not fit to output,
	// keep rest until playing again
	if (m_nInBuf > nWritten)
	{
		m_nInBuf = (m_nInBuf - nWritten);
		
		// this should support overlapped move
		::memmove(m_pDecodeBuffer->GetBegin(), m_pDecodeBuffer->GetBegin() + nWritten, m_nInBuf);
	}
	
}

// first write to output device
bool PlaybackHandler::initialOutput()
{
	// inital data to playback-buffer
	m_nInBuf = m_pModPlayer->Decode(m_pDecodeBuffer->GetBegin(), m_pDecodeBuffer->GetSize());
	
	// initial write to device-buffer
	qint64 nWritten = m_pDevOut->write(m_pDecodeBuffer->GetBegin(), m_nInBuf);
	if (nWritten == -1)
	{
		// failed
		return false;
	}
	m_Written += nWritten;

	// if all did not fit to output,
	// keep rest until playing again
	if (m_nInBuf > nWritten)
	{
		m_nInBuf = (m_nInBuf - nWritten);
		
		// this should support overlapped move
		::memmove(m_pDecodeBuffer->GetBegin(), m_pDecodeBuffer->GetBegin() + nWritten, m_nInBuf);
	}
	
	return true;
}


CModPlayer *PlaybackHandler::GetPlayer(CReadBuffer *fileBuffer) const
{
    if (fileBuffer->GetSize() < 32)
    {
        // nothing useful in the file..
        return nullptr;
    }
    
    CModPlayer *pModPlayer = nullptr;
	
	// some types have identifier somewhere middle of file..
    // just tell what we have for identifying it.
    CFileType type(fileBuffer->GetBegin(), fileBuffer->GetSize());
    
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
        
    case HEADERTYPE_XM:
        pModPlayer = new CFastTrackerPlayer(fileBuffer);
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
    
    // destroy old (if any)
    delete m_pFileBuffer;
	delete m_pModPlayer;
    delete m_pFile;

	// keep for debugging etc.
	//m_currentFilename = filename.right(filename.size() - filename.lastIndexOf('/'));
	m_currentFilename = filename;
	
    // open file and use memory-mapping
    // (leave buffering to OS)
    //
    m_pFile = new QFile(filename);
    qint64 nSize = m_pFile->size();
    uchar *pView = m_pFile->map(0, nSize);
    
    CFileType type(pView, nSize);
    if (type.m_enFileCategory == HEADERCAT_ARCHIVE)
    {
		// need to know which file in archive to decompress for playback..
		//m_pArchiveHandler->openArchive(filename);
		//m_pArchiveHandler->openArchive(m_pFile);
    }
	else if (type.m_enFileCategory == HEADERCAT_PACKER)
	{
		// single-file compression
		// -> just decrunch to buffer
	
		//m_pArchiveHandler->openArchive(filename);
		//m_pArchiveHandler->openArchive(pView, nSize);
		
		//m_pFileBuffer = m_pArchiveHandler->decrunchToBuffer(pView, nSize);
	}
	else
	{
		// module file without compression, just use as-is.
		//
		// use buffer as interface to accessing memory-mapped file:
		// OS will generate pagefaults as needed
		//
		m_pFileBuffer = new CReadBuffer(pView, nSize);
	}

	// format-specific handling,
	// fileformat, playback-to-buffer etc.
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
	
	// get decoding context: 
	// player should keep position/status information,
	// we want it to control position (if possible..)
	//
	// TODO: determine how we are able to output
	// (e.g. 256 tracker channels -> 2 channels in stereo,
	// 8/16-bit width?)
	// and check that device supports what is possible..
	//
	m_pCtx = m_pModPlayer->PrepareDecoder();
    
    // TODO: get info on what is suitable format for playing..
    // (what is supported by player/format/module..)
    //QAudioFormat format = pModPlayer->GetOutputFormat();
	// placeholder.. set output format
	// these will need changing later:
	// due to module-format support
	// and device-support we may need something or other..
    QAudioFormat format;
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");
	format.setSampleRate(m_pCtx->sampleRate());
	format.setChannels(m_pCtx->channelCount());
	format.setSampleSize(m_pCtx->sampleSize());
	format.setSampleType(QAudioFormat::SignedInt);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (info.isNull() == true)
    {
        emit error("Failed to get default audio output");
		return;
    }
	if (info.isFormatSupported(format) == false)
	{
		emit error("Output format not supported");
		return;
	}

	// expect 2-channels and 8-bit samples at 44.1kHz,
    // get proper values later, expecting 1s buffer to be enough for now..
	//
    size_t nBuffer = (format.sampleRate() * format.channels() * (format.sampleSize()/8));
    m_pDecodeBuffer->PrepareBuffer(nBuffer, false);

	// get device for output
	m_pAudioOut = new QAudioOutput(format, this);
	connect(m_pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(onAudioState(QAudio::State)));
	connect(m_pAudioOut, SIGNAL(notify()), this, SLOT(onPlayNotify()));

	if (m_pAudioOut->bufferSize() < nBuffer)
	{
		m_pAudioOut->setBufferSize(nBuffer);
	}
    m_pAudioOut->setNotifyInterval(250); // 250ms

	// push-mode (we decode to buffer and push to device),
	// we might need different thread for no-gaps playback..
	m_pDevOut = m_pAudioOut->start();

	if (initialOutput() == false)
	{
		// failed, cleanup
		stopPlay(); // status message
		emit error("Failed on audiodevice write(): " + filename);
	}
}	
    
 
