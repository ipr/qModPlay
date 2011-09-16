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

class CFastTrackerPlayer : public CModPlayer
{
protected:

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
    uint16_t m_version;
    
    std::string m_moduleName;
    std::string m_trackerName;
    
public:
    CFastTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CFastTrackerPlayer();

    virtual bool ParseFileInfo();
};

#endif // FASTTRACKERPLAYER_H
