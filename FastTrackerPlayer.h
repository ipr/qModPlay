//////////////////////////////////////
//
// CFastTrackerPlayer :
//
// just interface for now..
//
// Based on documentation By Mr.H of Triton in 1994
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef FASTTRACKERPLAYER_H
#define FASTTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>

// TODO: should be CFastTracker2Player.. old is more protracker-like file..
class CFastTrackerPlayer : public CModPlayer
{
protected:
    
    // empty patterns are not saved in file
    // so we always need to allocate
    // enough in case empty pattern is referenced somehow..
    class FTPattern
    {
    public:
        FTPattern()
            : m_patternData()
            , m_patternHeaderLen(9)
            , m_packingType(0)
            , m_rowCount(64)
            , m_packedSize(0) // actually, 64*chcount, but we don't know that yet..
        {
            m_patternData.m_nLen = m_rowCount; // 64 bytes
            m_patternData.m_pBuf = uint8_t[m_patternData.m_nLen];
            ::memset(m_patternData.m_pBuf, 0x80, m_patternData.m_nLen);
        }
        ~FTPattern()
        {
            delete m_patternData.m_pBuf;
        }
        
        // pre-allocate in case will be empty..
        // size: 64 bytes
        // initial value $80 (in case empty)
        bufferedData_t m_patternData;
        
        uint32_t m_patternHeaderLen;
        uint8_t m_packingType; // always 0 ?
        uint16_t m_rowCount;
        uint16_t m_packedSize;
        
    };
    // size: number of channels
    // TODO: is this wrong? 
    FTPattern *m_patterns;
    

    bufferedData_t m_patternOrders;
    
    uint16_t m_songLength;
    uint16_t m_restartPosition;
    uint16_t m_channelCount;
    uint16_t m_patternCount;
    uint16_t m_instrumentCount;
    uint16_t m_flags;
    uint16_t m_defaultTempo;
    uint16_t m_defaultBPM;
    
    // no need to keep, just for debugging now..
    uint32_t m_headerSize;
    
    uint8_t m_mystery;
    //uint16_t m_version;
    uint8_t m_version;
    uint8_t m_revision;
    
    std::string m_moduleName;
    std::string m_trackerName;
    
    // TODO: check needed parameters..
    void OnPatternData();
    
public:
    CFastTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CFastTrackerPlayer();

    virtual bool ParseFileInfo();
};

#endif // FASTTRACKERPLAYER_H
