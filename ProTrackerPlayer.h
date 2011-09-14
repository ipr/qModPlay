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
        
        FMT_ST15, // 15 samples, no identification..
        FMT_MK31, // "M.K.", 31 samples, upto 64 patterns?
        FMT_MK64 // "M!K!", 31 samples, 64 patterns or more
        
        /*
        FMT_CHN4 // "4CHN"
        FMT_CHN6 // "6CHN"
        FMT_CHN8 // "8CHN"
        FMT_FLT4 // Startrekker "FLT4"
        FMT_FLT8 // Startrekker "FLT8"
        */
        
    };
    PTModuleFormat m_enFormat;

    class PTPatternData
    {
    public:
        PTPatternData() {}
        ~PTPatternData() {}

        // keep notes of each channel,        
        // limited to 4 channels
        // -> simpler way..
        /*
        uint8_t *m_chan1;
        uint8_t *m_chan2;
        uint8_t *m_chan3;
        uint8_t *m_chan4;
        */
    };
    
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

    // differs according to variation of the format..
    // this is determined from song-positions array
    uint8_t m_nPatternCount;
    PTPatternData *m_pPatternData;

    // actually fixed-length array of 128 bytes
    bufferedData_t m_songPositions;
    
    // sample-count is usually fixed 31 samples
    // but variations of same format may different..
    size_t m_nSampleCount;
    PTSampleInfo *m_pSampleInfo;

    // this is set to 127 usually but NoiseTracker uses for restart point..?
    uint8_t m_mysterybyte; // .. name says it all ..
    
    uint8_t m_songLength; // 1-128
    std::string m_songname;
    
public:
    CProTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CProTrackerPlayer();

    virtual bool ParseFileInfo();

    // TODO: check parameters later..    
    virtual DecodeCtx *PrepareDecoder();
    
    // something like this to decode into buffer
    // for device-independent output in playback
    virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize);
    
};

#endif // PROTRACKERPLAYER_H
