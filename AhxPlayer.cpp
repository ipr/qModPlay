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
    , m_enAhxFormat(Unknown)
    , m_subsongList(nullptr)
    , m_positionList(nullptr)
    , m_trackListData(nullptr)
    , m_trackZero(nullptr)
    , m_sampleData(nullptr)
{
    // needed anyway?
    m_trackZero = new uint8_t[192];
    ::memset(m_trackZero, 0, 192);
}

CAhxPlayer::~CAhxPlayer()
{
    delete [] m_sampleData;
    delete m_trackZero;
    delete [] m_trackListData;
    delete [] m_positionList;
    delete [] m_subsongList;
}

bool CAhxPlayer::ParseFileInfo()
{
    m_pFileData->SetCurrentPos(0);
    uint8_t *pBuf = m_pFileData->GetNext(4);
    if (pBuf[0] == 'T' && pBuf[1] == 'H' && pBuf[2] == 'X')
    {
        // check fourth byte also (AHX0, AHX1)
        if (pBuf[3] == 0x00)
        {
            m_enAhxFormat = AHX0;
        }
        else if (pBuf[3] == 0x01)
        {
            m_enAhxFormat = AHX1;
        }
    }
    
    if (m_enAhxFormat == Unknown)
    {
        // unsupported format
        return false;
    }
    
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
    
    // subsong list
    if (m_subsongCount > 0)
    {
        m_subsongList = AHXSubsongEntry_t[0];
        for (int i = 0; i < m_subsongCount; i++)
        {
            // just a value, nothing more to it?
            m_subsongList[i].m_song = ReadBEUI16();
        }
    }
    
    // position list
    m_positionList = new AHXPositionEntry_t[m_posListLen];
    for (int i = 0; i < m_posListLen; i++)
    {
        // 8 bytes, 4 sets (for each audiochannel), pair of bytes in each set
        m_positionList[i].setFromArray(m_pFileData->GetNext(8));
    }
    
    // track list (zero count is valid also)
    if (m_trackCount > 0)
    {
        size_t n = m_trackCount*m_trackLen;
        m_trackListData = new AHXTrackEntry_t[n];
        for (int i = 0; i < n; i++)
        {
            m_trackListData[i].setFromArray(m_pFileData->GetNext(3));
        }
    }
    
    // samples
    if (m_sampleCount > 0)
    {
        m_sampleData = AHXSampleEntry_t[m_sampleCount];
        for (int i = 0; i < m_sampleCount; i++)
        {
            m_sampleData[i].setFromArray(m_pFileData->GetNext(22));
            
            // get length of playlist
            uint8_t playlistLen = m_sampleData[i].getPlaylistLen();
            
            // TODO: playlist data..
        }
    }
    
    
    return true;
}

