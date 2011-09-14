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


bool CMadTracker2Player::ParseDrumsDatas(size_t nLen)
{
    m_drumsPatternCount = m_pFileData->NextUI2();
    
    /*
    if (nLen / 274 != m_drumsPatternCount)
    {
        // invalid count or bug?
        return false;
    }
    */
    
    m_pDrumsData = new MT2Drums[m_drumsPatternCount];
    
    for (int i = 0; i < m_drumsPatternCount; i++)
    {
        m_pDrumsData[i].m_drumsSampleCount = 8; // always 8 ??
        m_pDrumsData[i].m_drumsSamples = new uint16_t[m_pDrumsData[i].m_drumsSampleCount];
        for (int j = 0; j < m_pDrumsData[i].m_drumsSampleCount; j++)
        {
            m_pDrumsData[i].m_drumsSamples[j] = m_pFileData->NextUI2();
        }
        
        m_pDrumsData[i].m_patternOrder.m_nLen = 256; 
        m_pDrumsData[i].m_patternOrder.m_pBuf = new uint8_t[m_pDrumsData[i].m_patternOrder.m_nLen];
    
        m_pFileData->NextArray(m_pDrumsData[i].m_patternOrder.m_pBuf, m_pDrumsData[i].m_patternOrder.m_nLen);
    }
    return true;
}

// IFF-style chunk structure
bool CMadTracker2Player::ParseAdditionalDatas(size_t nLen)
{
    while (m_pFileData->IsEnd() == false)
    {
        // common IFF-tag style chunk ID
        uint32_t chunkID = m_pFileData->NextUI4();
        
        // length always given
        uint32_t chunkLen = m_pFileData->NextUI4();
        
        size_t nPos = m_pFileData->GetCurrentPos();
        
        // note!! three byte identifiers also!!
        // force NULL instead of space into identifier where only three characters!
        // this is not true IFF as it should be space..
        //
        if (chunkID == IFFTag("TRKS"))
        {
            m_masterVolume = m_pFileData->NextUI2();
            
            // assuming there's more than one instance here (one per track..)
            // should be same as m_trackCount in file header?
            //size_t nTracks = ((chunkLen - 2) / sizeof(MT2TracksDatas_t));
            //MT2TracksDatas_t *pTrkData = (MT2TracksDatas_t*)m_pFileData->GetAtCurrent();

            // quick simple copy for parsing later..
            m_pTracksData = new MT2TracksData[m_trackCount];
            for (int i = 0; i < m_trackCount; i++)
            {
                m_pFileData->NextArray(&(m_pTracksData[i].m_trackData), sizeof(MT2TracksDatas_t));
            }
        }
        else if (chunkID == IFFTag("MSG\0"))
        {
            m_showMessage = m_pFileData->NextUI1();
            m_messageText.assign((char*)m_pFileData->GetAtCurrent(), chunkLen-1);
        }
        else if (chunkID == IFFTag("SUM\0"))
        {
            m_pFileData->NextArray(m_summaryMask, 6);
            m_summaryContent.m_nLen = (chunkLen - 6);
            m_summaryContent.m_pBuf = new uint8_t[m_summaryContent.m_nLen];
            m_pFileData->NextArray(m_summaryContent.m_pBuf, m_summaryContent.m_nLen);
        }
        
        // if chunk is not supported, skip to next? or fail?
        m_pFileData->SetCurrentPos(nPos + chunkLen);
    }
    
    return true;
}

bool CMadTracker2Player::ParsePatterns(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseDrumsPatterns(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseAutomation(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseInstruments(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseSamples(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseGroups(uint8_t *pData, size_t nLen)
{
    return false;
}
bool CMadTracker2Player::ParseSamplesDatas(uint8_t *pData, size_t nLen)
{
    return false;
}


/////////// public


CMadTracker2Player::CMadTracker2Player(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_patternOrders()
    , m_pDrumsData(nullptr)
    , m_pTracksData(nullptr)
    //, m_additionalsData()
    , m_summaryContent()
{
}

CMadTracker2Player::~CMadTracker2Player()
{
    delete m_summaryContent.m_pBuf;
    //delete m_additionalsData.m_pBuf;
    delete [] m_pTracksData;
    delete [] m_pDrumsData;
    delete m_patternOrders.m_pBuf;
}

/* about fileformat..

  Header
  if (drums) Drums datas
  Additional datas
  Patterns (* number of patterns)
  if (drums) Drums Patterns (* number of drums patterns)
  Automation (* number of patterns)
  Instruments (* 255)
  Samples (* 256)
  Groups (=parts of instruments that link to samples)
  Sample datas   

  
  - "additionals" has structure pretty close 
  to IFF-chunk format..
   
*/
bool CMadTracker2Player::ParseFileInfo()
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
    
    m_trackerName.assign((char*)m_pFileData->GetAtCurrent(), 32);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +32);
    
    m_moduleTitle.assign((char*)m_pFileData->GetAtCurrent(), 64);
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
    m_pFileData->NextArray(m_patternOrders.m_pBuf, m_patternOrders.m_nLen);

    size_t nLen = 0;
    size_t nPos = 0;
    
    // has additional structures
    nLen = m_pFileData->NextUI2();
    nPos = m_pFileData->GetCurrentPos();
    if (nLen > 0)
    {
        ParseDrumsDatas(nLen);
    }
    
    // has additional structures (IFF-style chunks)
    nLen = m_pFileData->NextUI4();
    nPos = m_pFileData->GetCurrentPos();
    if (nLen > 0)
    {
        ParseAdditionalDatas(nLen);
        m_pFileData->SetCurrentPos(nPos + nLen);
    }
    
    // TODO: rest of data..
    
    return true;
}

