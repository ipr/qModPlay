//////////////////////////////////////
//
// MadTracker2Player :
// player for Mad Tracker 2 modules 
// in (mostly) portable C++
//
// Official specification of MT20 format:
// http://www.madtracker.org/download/mt2.txt
//
// Don't know of other players now,
// the official Winamp plugin doesn't seem to work.. (binary only)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "MadTracker2Player.h"

// for buffer-wrapper..
#include "AnsiFile.h"


//////////// protected methods


bool MadTracker2Player::ParseDrumsDatas(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseAdditionalDatas(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParsePatterns(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseDrumsPatterns(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseAutomation(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseInstruments(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseSamples(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseGroups(uint8_t *pData, size_t nLen)
{
    return false;
}
bool MadTracker2Player::ParseSamplesDatas(uint8_t *pData, size_t nLen)
{
    return false;
}


/////////// public


MadTracker2Player::MadTracker2Player(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_patternOrders()
    //, m_drumsData()
    //, m_additionalsData()
{
}

MadTracker2Player::~MadTracker2Player()
{
    //delete m_drumsData.m_pBuf;
    //delete m_additionalsData.m_pBuf;
    delete m_patternOrders.m_pBuf;
}

/* about fileformat..

   much fixed-length stuff at start.
   
   "additionals" has structure pretty close 
   to IFF-chunk format..
   
*/
bool MadTracker2Player::ParseFileInfo()
{
    if (m_pFileData->NextUI4() != IFFTag("MT20"))
    {
        // not supported module
        return false;
    }
    
    // safe to ignore
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +4);

    m_version = m_pFileData->NextUI1();
    m_revision = m_pFileData->NextUI1();
    
    m_trackerName.assign(m_pFileData->GetAtCurrent(), 32);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +32);
    
    m_moduleTitle.assign(m_pFileData->GetAtCurrent(), 64);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +64);
    
    m_positionCount = m_pFileData->NextUI2();
    m_restartPosition = m_pFileData->NextUI2();
    m_patternCount = m_pFileData->NextUI2();
    m_trackCount = m_pFileData->NextUI2();
    m_samplesPerTick = m_pFileData->NextUI2();
    m_ticksPerLine = m_pFileData->NextUI1();
    m_linesPerBeat = m_pFileData->NextUI1();
    m_flags = m_pFileData->NextUI4();
    m_instrumentCount = m_pFileData->NextUI2();
    m_sampleCount = m_pFileData->NextUI2();
    
    // fixed length array
    m_patternOrders.m_nLen = 256;
    m_patternOrders.m_pBuf = new uint8_t[m_patternOrders.m_nLen];
    ::memcpy(m_patternOrders.m_pBuf, m_pFileData->GetAtCurrent(), m_patternOrders.m_nLen);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + m_patternOrders.m_nLen);

    size_t nLen = 0;
    
    // has additional structures
    nLen = m_pFileData->NextUI2();
    ParseDrumsDatas(m_pFileData->GetAtCurrent(), nLen);
    /*
    m_drumsData.m_nLen = m_pFileData->NextUI2();
    m_drumsData.m_pBuf = new uint8_t[m_drumsData.m_nLen];
    ::memcpy(m_drumsData.m_pBuf, m_pFileData->GetAtCurrent(), m_drumsData.m_nLen);
    */
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nLen);
    
    // has additional structures (IFF-style chunks)
    nLen = m_pFileData->NextUI4();
    ParseAdditionalDatas(m_pFileData->GetAtCurrent(), nLen);
    /*
    m_additionalsData.m_nLen = m_pFileData->NextUI4();
    m_additionalsData.m_pBuf = new uint8_t[m_additionalsData.m_nLen];
    ::memcpy(m_additionalsData.m_pBuf, m_pFileData->GetAtCurrent(), m_additionalsData.m_nLen);
    */
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + nLen);
    
    return true;
}

