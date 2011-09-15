//////////////////////////////////////
//
// CScreamTrackerPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef SCREAMTRACKERPLAYER_H
#define SCREAMTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>


class CScreamTrackerPlayer : public CModPlayer
{
protected:

    // just for memory aid if need becomes
    enum S3MFormatVersion
    {
        OLD_SIGNED_SAMPLES = 1,
        STANDARD_UNSIGNED_SAMPLES = 2
    };
 
    // size in bytes: m_orderCount*sizeof(uint8_t)
    uint8_t *m_pOrders;
    
    // size in bytes: m_instrumentCount*sizeof(uint16_t)
    uint16_t *m_pInstruments;

    // size in bytes: m_patternCount*sizeof(uint16_t)
    uint16_t *m_pPatterns;
    
    // fixed size 32-bytes
    uint8_t m_channelSettings[32];

    uint8_t m_masterVolume;
    uint8_t m_initialTempo;
    uint8_t m_initialSpeed;
    uint8_t m_globalVolume;
    
    uint16_t m_patternCount; // 
    uint16_t m_instrumentCount; // 
    uint16_t m_orderCount; //

    uint16_t m_special; // 
    uint16_t m_flags; // 
    
    uint16_t m_Cwtv; // "Created with tracker" version?
    uint16_t m_Ffv; // Fileformat version?

    std::string m_songName;
    
    // TODO: implement
    void ParseSampleData();
    
public:
    CScreamTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CScreamTrackerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // SCREAMTRACKERPLAYER_H
