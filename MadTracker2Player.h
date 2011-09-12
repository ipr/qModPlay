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

class CMadTracker2Player : public CModPlayer
{
protected:

    /* TODO: parse further..    
    class MT2Drums
    {
    public:
        MT2Drums()
            : m_data()
        {}
        ~MT2Drums()
        {}
        bufferedData_t m_data;
    };
    */
    
    // TODO: might remove these 
    // and replace with more proper structure..
    bufferedData_t m_patternOrders;
    //bufferedData_t m_drumsData;
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
    
    bool ParseDrumsDatas(uint8_t *pData, size_t nLen);
    bool ParseAdditionalDatas(uint8_t *pData, size_t nLen);
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
