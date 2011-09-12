//////////////////////////////////////
//
// CAhxPlayer :
// attempt at new (mostly) portable player
// for AHX0 and AHX1
//
// Based on documentation by: Stuart Caie
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "AhxPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

// use buffer only, can switch implementation easily then..
CAhxPlayer::CAhxPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

CAhxPlayer::~CAhxPlayer()
{
}

bool CAhxPlayer::ParseFileInfo()
{
    // check fourth byte also? (AHX0, AHX1)
    if (::memcmp(m_pFileData->GetBegin(), "THX", 3) != 0)
    {
        // unsupported format
        return false;
    }
    m_pFileData->SetCurrentPos(4);
    
    // actually, don't use this as offset..
    uint16_t byteskip = Swap2(m_pFileData->NextUI2());
    
    // some flags
    uint8_t temp = m_pFileData->NextUI1();
    m_trackSave = (temp & 0xF0);
    m_playSpeed = (temp & 0xF);
    
    m_posListLen = (Swap2(m_pFileData->NextUI2()) & 0xFF);
    m_restartPoint = Swap2(m_pFileData->NextUI2());
    
    m_trackLen = m_pFileData->NextUI1();
    m_trackCount = m_pFileData->NextUI1();
    m_sampleCount = m_pFileData->NextUI1();
    m_subsongCount = m_pFileData->NextUI1();
    
    
    return true;
}

