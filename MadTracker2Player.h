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


#ifndef MADTRACKER2PLAYER_H
#define MADTRACKER2PLAYER_H

#include "ModPlayer.h"

#include <string>

// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

// note: just guessing alignment to be 1,
// test&debug if it's true..
//
struct MT2TracksDatas_t
{
    uint16_t m_trackVolume;            // 
    uint8_t m_effectBuffer;            // 
    uint8_t m_outputTrack;             // 
    uint16_t m_trackEffectID;          // 
    uint16_t m_trackParameters[8];     // 
};

/* TODO: handle rest of data..
  
struct MT2PatternData_t
{};

struct MT2DrumsPatterns_t
{};
*/

#pragma pack(pop)


class CMadTracker2Player : public CModPlayer
{
protected:
    
    class MT2TracksData
    {
    public:
        MT2TracksData()
        {}
        ~MT2TracksData()
        {}
        
        MT2TracksDatas_t m_trackData;
    };

    class MT2Drums
    {
    public:
        MT2Drums()
            : m_patternOrder()
        {}
        ~MT2Drums()
        {
            delete m_patternOrder.m_pBuf;
        }
        uint16_t m_drumsSampleCount;
        uint16_t *m_drumsSamples;
        bufferedData_t m_patternOrder;
    };

    uint16_t m_drumsPatternCount;
    MT2Drums *m_pDrumsData;

    // additional datas, tracks
    uint16_t m_masterVolume;
    //size_t m_trackCount; // already in m_trackCount..?
    MT2TracksData *m_pTracksData;
    
    uint8_t m_summaryMask[6];
    bufferedData_t m_summaryContent;
    
    uint8_t m_showMessage; // read anyway..
    std::string m_messageText;
    
    // TODO: might remove these 
    // and replace with more proper structure..
    bufferedData_t m_patternOrders;
    //bufferedData_t m_additionalsData;
    
    uint16_t m_positionCount;
    uint16_t m_restartPosition;
    uint16_t m_patternCount;
    uint16_t m_trackCount;
    uint16_t m_samplesPerTick;
    uint8_t m_ticksPerLine;
    uint8_t m_linesPerBeat;
    uint32_t m_flags;
    uint16_t m_instrumentCount;
    uint16_t m_sampleCount;
    
    uint8_t m_version;
    uint8_t m_revision;
    
    // 32 char tracker name&version
    std::string m_trackerName;

    // 64 char module name
    std::string m_moduleTitle;
    
    bool ParseDrumsDatas(size_t nLen);
    bool ParseAdditionalDatas(size_t nLen);
    bool ParsePatterns(uint8_t *pData, size_t nLen);
    bool ParseDrumsPatterns(uint8_t *pData, size_t nLen);
    bool ParseAutomation(uint8_t *pData, size_t nLen);
    bool ParseInstruments(uint8_t *pData, size_t nLen);
    bool ParseSamples(uint8_t *pData, size_t nLen);
    bool ParseGroups(uint8_t *pData, size_t nLen);
    bool ParseSamplesDatas(uint8_t *pData, size_t nLen);
    
public:
    CMadTracker2Player(CReadBuffer *pFileData);
    virtual ~CMadTracker2Player();
    
    virtual bool ParseFileInfo();
};

#endif // MADTRACKER2PLAYER_H
