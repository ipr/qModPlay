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
    , m_trackZero(nullptr)
    , m_trackListData(nullptr)
{
    // needed anyway?
    m_trackZero = new uint8_t[192];
    ::memset(m_trackZero, 0, 192);
}

CAhxPlayer::~CAhxPlayer()
{
    delete m_trackListData;
    delete m_trackZero;
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
    uint16_t byteskip = ReadBEUI16();
    
    // some flags
    uint8_t temp = ReadUI8();
    m_trackSave = (temp & 0xF0);
    m_playSpeed = (temp & 0x0F);
    
    m_posListLen = (ReadBEUI16() & 0xFF); // LEN
    m_restartPoint = ReadBEUI16();
    
    m_trackLen = ReadUI8(); // TRL
    m_trackCount = ReadUI8(); // TRK
    m_sampleCount = ReadUI8();
    m_subsongCount = ReadUI8();
    
    // TODO:
    // subsong list
    for (int i = 0; i < m_subsongCount; i++)
    {
    }
    
    // TODO:
    // position list
    for (int i = 0; i < m_posListLen; i++)
    {
        // 8 bytes, 4 sets (for each audiochannel)
    }
    
    // track list (zero count is valid also)
    if (m_trackCount > 0)
    {
        m_trackListData = new AHXTrackEntry_t[m_trackCount*m_trackLen];
        for (int i = 0; i < m_trackCount*m_trackLen; i++)
        {
            m_trackListData[i].setFromArray(m_pFileData->GetAtCurrent());
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 3);
        }
    }
    
    // samples
    for (int i = 0; i < m_sampleCount; i++)
    {
    }
    
    return true;
}

