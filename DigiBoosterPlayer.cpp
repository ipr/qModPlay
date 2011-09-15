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


//////////// protected methods

// note: where is this in file?
// similar to protracker except at offset 1572 ?
//
// (spec is rather vague here..)
// -> see protracker format for details and get back to this..
//
void CDigiBoosterPlayer::OnPackedPattern()
{
    // song data (patterns?)
    // -> storing similar to protracker?

    // fixed positions so far..
    //auto pPos = m_pFileData->GetAt(1572);
    m_pFileData->SetCurrentPos(1572);
    
    // size of data?
    //m_channelCount * m_patternsCount * sizeof(pattern) ??
    
    // starts with size of pattern data
    uint16_t patternDataSize = Swap2(m_pFileData->NextUI2());

    // fixed-size array of bitmasks
    m_pFileData->NextArray(m_eventMasks, 64);
    
    m_songData.m_nLen = (patternDataSize - 64);
    m_pFileData->NextArray(m_songData.m_pBuf, m_songData.m_nLen);

    // next position?
    //m_pFileData->SetCurrentPos(1572 + patternDataSize + 2);
    
}


/////////// public

CDigiBoosterPlayer::CDigiBoosterPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_orders()
    , m_songData()
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
    
    delete m_songData.m_pBuf;
    delete m_orders.m_pBuf;
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
    m_orders.m_nLen = 128;
    m_orders.m_pBuf = new uint8_t[m_orders.m_nLen];
    
    m_pFileData->SetCurrentPos(48); // fixed offset..
    m_pFileData->NextArray(m_orders.m_pBuf, m_orders.m_nLen);
    
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
    // past fixed-sized name-area
    m_pFileData->SetCurrentPos(642 + 31*30);

    // song data
    // -> storing similar to protracker?
    
    // verify position (not updated constantly..)
    /*
    if (m_pFileData->GetCurrentPos() != 1572)
    {
        // bug?? offsets&sizes don't match?
    }
    */
    
    OnPackedPattern();
    
    return true;    
}

