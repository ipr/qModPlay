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
{
}

CImpulseTrackerPlayer::~CImpulseTrackerPlayer()
{
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

    // 64 bytes: channel volume (for each channel)
    
    return true;
}

