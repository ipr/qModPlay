//////////////////////////////////////
//
// CImpulseTrackerPlayer :
//
// Based on documentation by unknown..
// (sorry, no name on the file..)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "ImpulseTrackerPlayer.h"


CImpulseTrackerPlayer::CImpulseTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pOrders(nullptr)
    , m_pInstruments(nullptr)
    , m_pSamples(nullptr)
    , m_pPatterns(nullptr)
{
}

CImpulseTrackerPlayer::~CImpulseTrackerPlayer()
{
    delete m_pPatterns;
    delete m_pSamples;
    delete m_pInstruments;
    delete m_pOrders;
}

/*
 note: originates on x86 PC and uses little-endian 
 byteorder in files..
 
*/
bool CImpulseTrackerPlayer::ParseFileInfo()
{
    if (ReadLEUI32() != IFFTag("IMPM"))
    {
        // not impulsetracker module
        return false;
    }
    
    // 26 byte NULL-terminated string
    m_songName.assign((char*)m_pFileData->GetAtCurrent(), 26);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 26);
 
    m_patternRowHighlight = ReadLEUI16();

    m_orderCount = ReadLEUI16();
    m_instrumentCount = ReadLEUI16();
    m_sampleCount = ReadLEUI16();
    m_patternCount = ReadLEUI16();
    
    m_Cwtv = ReadLEUI16();
    m_Cmwt = ReadLEUI16();

    m_flags = ReadLEUI16();
    m_special = ReadLEUI16();

    m_globalVolume = ReadUI8();
    m_mixVolume = ReadUI8();
    m_initialSpeed = ReadUI8();
    m_initialTempo = ReadUI8();

    m_panningSeparation = ReadUI8();
    m_PitchWheelDepth = ReadUI8();
    
    uint16_t msgLen = ReadLEUI16();
    uint32_t msgOffset = ReadLEUI32();

    // skip four bytes (reserved)
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 4);

    // 64 bytes: channel panning (for each channel)
    m_pFileData->NextArray(m_channelPanning, 64);

    // 64 bytes: channel volume (for each channel)
    m_pFileData->NextArray(m_channelVolume, 64);
    
    m_pOrders = new uint8_t[m_orderCount];
    m_pFileData->NextArray(m_pOrders, m_orderCount*sizeof(uint8_t));
    
    m_pInstruments = new uint32_t[m_instrumentCount];
    m_pFileData->NextArray(m_pInstruments, m_instrumentCount*sizeof(uint32_t));
    if (m_bBigEndian)
    {
        for (int i = 0; i < m_instrumentCount; i++)
        {
            m_pInstruments[i] = Swap4(m_pInstruments[i]);
        }
    }
    
    m_pSamples = new uint32_t[m_sampleCount];
    m_pFileData->NextArray(m_pSamples, m_sampleCount*sizeof(uint32_t));
    if (m_bBigEndian)
    {
        for (int i = 0; i < m_sampleCount; i++)
        {
            m_pSamples[i] = Swap4(m_pSamples[i]);
        }
    }
    
    m_pPatterns = new uint32_t[m_patternCount];
    m_pFileData->NextArray(m_pPatterns, m_patternCount*sizeof(uint32_t));
    if (m_bBigEndian)
    {
        for (int i = 0; i < m_patternCount; i++)
        {
            m_pPatterns[i] = Swap4(m_pPatterns[i]);
        }
    }
    
    // TODO: sample/instrument handling etc.
    
    return true;
}

DecodeCtx *CImpulseTrackerPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
    
    return m_pDecodeCtx;
}

// TODO:
size_t CImpulseTrackerPlayer::DecodePlay(void *pBuffer, const size_t nBufSize)
{
    return 0;
}

