//////////////////////////////////////
//
// CProTrackerPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef PROTRACKERPLAYER_H
#define PROTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>

class CProTrackerPlayer : public CModPlayer
{
protected:
    
    class PTSampleInfo
    {
    public:
        PTSampleInfo() {}
        ~PTSampleInfo() {}

        std::string m_name;
        uint16_t m_length;
        uint8_t m_finetune; // half-byte actually..
        uint8_t m_volume; // 0..64
        
        uint16_t m_repeatPoint; // note: keep offset in bytes
        uint16_t m_repeatLength; // note: keep offset in bytes
    };
    
    // fixed, 31 samples
    PTSampleInfo *m_pSampleInfo;
    
    // actually fixed-length array of 128 bytes
    bufferedData_t m_songPositions;
    
    // this is set to 127 usually but NoiseTracker uses for restart..?
    uint8_t m_mysterybyte; // .. name says it all ..
    
    uint8_t m_songLength; // 1-128
    std::string m_songname;
    
public:
    CProTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CProTrackerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // PROTRACKERPLAYER_H
