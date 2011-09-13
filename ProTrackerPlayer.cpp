//////////////////////////////////////
//
// CProTrackerPlayer :
// attempt at creating (yet another) player 
// for ProTracker modules..
// 
//
// Based on documentation by: 
// Lars Hamre, Steinar Midtskogen, 
// Norman Lin, Mark Cox, Peter Hanning,
// Marc Espie, and Thomas Meyer
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "ProTrackerPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CProTrackerPlayer::CProTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_enFormat(FMT_Unknown)
    , m_pSampleInfo(nullptr)
    , m_pPatternData(nullptr)
    , m_nSampleCount(0)
    , m_songPositions()
    , m_nPatternCount(0)
{
}

CProTrackerPlayer::~CProTrackerPlayer()
{
    delete m_songPositions.m_pBuf;
    delete [] m_pPatternData;
    delete [] m_pSampleInfo;
}

// TODO: file type identification ??
// no proper header in format?
//
bool CProTrackerPlayer::ParseFileInfo()
{
    if (m_pFileData->GetSize() < 1084)
    {
        // not enough data for a ProTracker module
        // (fixed-size before this, identifier here)
        return false;
    }
    
    // note: might not exist in some cases
    // or variations may have something else..
    // may be "FLT4" or "FLT8" for Startrekker module?
    //
    uint8_t *pIdPos = m_pFileData->GetAt(1080);
    if (::memcmp(pIdPos, "M.K.", 4) == 0)
    {
        // fixed-size of 31 samples
        m_enFormat = FMT_MK31;
        m_nSampleCount = 31;
    }
    /*
    else if (::memcmp(pIdPos, "M!K!", 4) == 0)
    {
        m_enFormat = FMT_MK64;
        m_nSampleCount = 31;
    }
    */
    else
    {
        // .. not ProTracker module?
        // (not supported anyway, at least not yet..)
        // -> another way to detect which format?
        
        // old-style format (soundtracker?)
        // has fixed-size of 15 samples ?
        //m_enFormat = FMT_ST15;
        //m_nSampleCount = 15;
        return false;
    }
    
    // starts with song name? (fixed-length)
    m_songname.assign((char*)m_pFileData->GetBegin(), 20);
    m_pFileData->SetCurrentPos(20);
    
    // fixed amount of fixed-length sample-info..
    //
    // note: may be only 15 if format ID is not given,
    // how to identify fileformat then??
    //
    m_pSampleInfo = new PTSampleInfo[m_nSampleCount];
    for (int i = 0; i < m_nSampleCount; i++)
    {
        m_pSampleInfo[i].m_name.assign((char*)m_pFileData->GetAtCurrent(), 22);
        m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +22);
        
        m_pSampleInfo[i].m_length = Swap2(m_pFileData->NextUI2());
        m_pSampleInfo[i].m_finetune = (int8_t)(m_pFileData->NextUI1() & 0x0F); // signed nibble
        m_pSampleInfo[i].m_volume = m_pFileData->NextUI1();

        // note: keep offset in bytes for simplicity later
        m_pSampleInfo[i].m_repeatPoint = Swap2(m_pFileData->NextUI2()) * 2;
        m_pSampleInfo[i].m_repeatLength = Swap2(m_pFileData->NextUI2()) * 2;
    }

    m_songLength = m_pFileData->NextUI1();
    m_mysterybyte = m_pFileData->NextUI1(); // NoiseTracker uses for restart position?
    
    m_songPositions.m_nLen = 128;
    m_songPositions.m_pBuf = new uint8_t[m_songPositions.m_nLen];
    m_pFileData->NextArray(m_songPositions.m_pBuf, m_songPositions.m_nLen);
    
    // locate highest pattern number
    // (also count of patterns)
    m_nPatternCount = 0;
    for (int i = 0; i < m_songPositions.m_nLen; i++)
    {
        if (m_songPositions.m_pBuf[i] > m_nPatternCount)
        {
            // index to count of patterns
            m_nPatternCount = (m_songPositions.m_pBuf[i] + 1);
        }
    }
    
    // does this way complicate playback..?
    //m_pPatternData = new PTPatternData[m_nPatternCount];
    
    
    return true;
}

