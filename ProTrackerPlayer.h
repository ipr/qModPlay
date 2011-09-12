//////////////////////////////////////
//
// CProTrackerPlayer :
// attempt at creating (yet another) player 
// for ProTracker modules..
// 
//
// Based on documentation by: 
// Lars Hamre, Steinar Midtskogen, 
// Norman Lin, Mark Cox, Peter Hanning,
// Marc Espie, and Thomas Meyer
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
    
    // variations on ProTracker module format..
    enum PTModuleFormat
    {
        FMT_Unknown, // failed to determine
        
        FMT_MK31, // 31 samples, "M.K."
        FMT_MK64 // 64 samples or more, "M!K!"
        
        /*
        FMT_CHN6 // "6CHN"
        FMT_CHN8 // "8CHN"
        FMT_FLT4 // Startrekker "FLT4"
        FMT_FLT8 // Startrekker "FLT8"
        */
        
    };
    PTModuleFormat m_enFormat;
    
    class PTSampleInfo
    {
    public:
        PTSampleInfo() {}
        ~PTSampleInfo() {}

        std::string m_name;
        uint16_t m_length;
        int8_t m_finetune; // half-byte actually.. signed
        uint8_t m_volume; // 0..64
        
        uint16_t m_repeatPoint; // note: keep offset in bytes
        uint16_t m_repeatLength; // note: keep offset in bytes
    };
    
    // fixed, 31 samples
    PTSampleInfo *m_pSampleInfo;
    
    // actually fixed-length array of 128 bytes
    bufferedData_t m_songPositions;
    
    // this is set to 127 usually but NoiseTracker uses for restart point..?
    uint8_t m_mysterybyte; // .. name says it all ..
    
    uint8_t m_songLength; // 1-128
    std::string m_songname;
    
public:
    CProTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CProTrackerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // PROTRACKERPLAYER_H
