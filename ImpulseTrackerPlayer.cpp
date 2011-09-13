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

// for buffer-wrapper..
#include "AnsiFile.h"


CImpulseTrackerPlayer::CImpulseTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pOrders(nullptr)
{
}

CImpulseTrackerPlayer::~CImpulseTrackerPlayer()
{
    delete m_pOrders;
}

/*
 note: originates on x86 PC so uses little-endian 
 byteorder in files??
 (verify..)
*/

bool CImpulseTrackerPlayer::ParseFileInfo()
{
    if (m_pFileData->NextUI4() != IFFTag("IMPM"))
    {
        // not impulsetracker module
        return false;
    }
    
    // 26 byte NULL-terminated string
    m_songName.assign((char*)m_pFileData->GetAtCurrent(), 26);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 26);
 
    m_patternRowHighlight = m_pFileData->NextUI2();

    m_orderCount = m_pFileData->NextUI2();
    m_instrumentCount = m_pFileData->NextUI2();
    m_sampleCount = m_pFileData->NextUI2();
    m_patternCount = m_pFileData->NextUI2();
    
    m_Cwtv = m_pFileData->NextUI2();
    m_Cmwt = m_pFileData->NextUI2();

    m_flags = m_pFileData->NextUI2();
    m_special = m_pFileData->NextUI2();

    m_globalVolume = m_pFileData->NextUI1();
    m_mixVolume = m_pFileData->NextUI1();
    m_initialSpeed = m_pFileData->NextUI1();
    m_initialTempo = m_pFileData->NextUI1();

    m_panningSeparation = m_pFileData->NextUI1();
    m_PitchWheelDepth = m_pFileData->NextUI1();
    
    uint16_t msgLen = m_pFileData->NextUI2();
    uint32_t msgOffset = m_pFileData->NextUI4();

    // skip four bytes (reserved)
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 4);

    // 64 bytes: channel panning (for each channel)
    ::memcpy(m_channelPanning, m_pFileData->GetAtCurrent(), 64);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 64);

    // 64 bytes: channel volume (for each channel)
    ::memcpy(m_channelVolume, m_pFileData->GetAtCurrent(), 64);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 64);
    
    size_t nBytes = 0; // hate repeating myself..
    
    m_pOrders = new uint8_t[m_orderCount];
    nBytes = m_orderCount*sizeof(uint8_t);
    ::memcpy(m_pOrders, m_pFileData->GetAtCurrent(), nBytes);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nBytes);
    
    m_pInstruments = new uint32_t[m_instrumentCount];
    nBytes = m_instrumentCount*sizeof(uint32_t);
    ::memcpy(m_pInstruments, m_pFileData->GetAtCurrent(), nBytes);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nBytes);
    
    m_pSamples = new uint32_t[m_sampleCount];
    nBytes = m_sampleCount*sizeof(uint32_t);
    ::memcpy(m_pSamples, m_pFileData->GetAtCurrent(), nBytes);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nBytes);
    
    m_pPatterns = new uint32_t[m_patternCount];
    nBytes = m_patternCount*sizeof(uint32_t);
    ::memcpy(m_pPatterns, m_pFileData->GetAtCurrent(), nBytes);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nBytes);
    
    
    return true;
}

