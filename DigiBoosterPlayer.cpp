//////////////////////////////////////
//
// CDigiBoosterPlayer :
// player for DigiBooster modules
// in (mostly) portable C++
//
// Based on documentation by: Claudio Matsuoka
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "DigiBoosterPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CDigiBoosterPlayer::CDigiBoosterPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_Orders()
    , m_SongData()
    , m_pSampleLength(nullptr)
    , m_pSampleLoopStart(nullptr)
    , m_pSampleLoopLength(nullptr)
    , m_pSampleVolumes(nullptr)
    , m_pSampleFinetunes(nullptr)
    , m_songName()
    , m_pSampleNames(nullptr)
{
}

CDigiBoosterPlayer::~CDigiBoosterPlayer()
{
    delete [] m_pSampleNames;
    delete m_pSampleLength;
    delete m_pSampleLoopStart;
    delete m_pSampleLoopLength;
    delete m_pSampleVolumes;
    delete m_pSampleFinetunes;
    
    delete m_SongData.m_pBuf;
    delete m_Orders.m_pBuf;
}

bool CDigiBoosterPlayer::ParseFileInfo()
{
    auto pPos = m_pFileData->GetBegin();
    if (::memcmp(pPos, "DIGI Booster module\0", 20) != 0)
    {
        // unsupported format
        return false;
    }
    
    // version string
    ::memcpy(m_versionName, pPos + 20, 4);
    m_versionNumber = m_pFileData->GetAt(24)[0];
    
    m_channelCount = m_pFileData->GetAt(25)[0];

    // TODO: pack enable? what size?
    // assuming byte..
    m_packEnable = m_pFileData->GetAt(26)[0];
    
    // unknown segment..
    
    m_patternsCount = (m_pFileData->GetAt(46)[0]) -1;
    m_orderCount = (m_pFileData->GetAt(47)[0]) -1;

    // should use value from above?
    m_Orders.m_nLen = 128;
    m_Orders.m_pBuf = new uint8_t[m_Orders.m_nLen];
    ::memcpy(m_Orders.m_pBuf, m_pFileData->GetAt(48), m_Orders.m_nLen);
    
    // always 31 values
    m_pSampleLength = new uint32_t[31];
    m_pSampleLoopStart = new uint32_t[31];
    m_pSampleLoopLength = new uint32_t[31];
    m_pSampleVolumes = new uint8_t[31];
    m_pSampleFinetunes = new uint8_t[31];
    
    uint32_t *pSmpLen = (uint32_t*)m_pFileData->GetAt(176);
    for (int i = 0; i < 31; i++)
    {
        m_pSampleLength[i] = Swap4(pSmpLen[i]);
    }
    uint32_t *pSmpLoopStart = (uint32_t*)m_pFileData->GetAt(300);
    for (int i = 0; i < 31; i++)
    {
        m_pSampleLoopStart[i] = Swap4(pSmpLoopStart[i]);
    }
    uint32_t *pSmpLoopLen = (uint32_t*)m_pFileData->GetAt(424);
    for (int i = 0; i < 31; i++)
    {
        m_pSampleLoopLength[i] = Swap4(pSmpLoopLen[i]);
    }
    pPos = m_pFileData->GetAt(548);
    ::memcpy(m_pSampleVolumes, pPos, 31);
    pPos = m_pFileData->GetAt(579);
    ::memcpy(m_pSampleFinetunes, pPos, 31);

    // fixed length name for song
    m_songName.assign((char*)m_pFileData->GetAt(610), 32);

    // fixed sized array of fixed-length strings
    m_pSampleNames = new std::string[31];
    pPos = m_pFileData->GetAt(642);
    for (int i = 0; i < 31; i++)
    {
        m_pSampleNames[i].assign((char*)pPos, 30);
        pPos = (pPos + 30);
    }

    // song data
    // -> storing similar to protracker?
    
    /*
    pPos = m_pFileData->GetAt(1572);
    m_SongData.m_nLen = ??;
    m_SongData.m_pBuf = new uint8_t[m_SongData.m_nLen];
    ::memcpy(m_SongData.m_pBuf, pPos, m_SongData.m_nLen);
    */
    
    return true;    
}

