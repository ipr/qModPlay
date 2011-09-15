//////////////////////////////////////
//
// CImpulseTrackerPlayer :
//
// Based on documentation by unknown..
// (sorry, no name on the file..)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef IMPULSETRACKERPLAYER_H
#define IMPULSETRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>


class CImpulseTrackerPlayer : public CModPlayer
{
protected:
    
    // special case values in orders
    enum ITOrders
    {
        END_OF_SONG = 0xFF,
        SKIP_TO_NEXT = 0xFE
    };
    
    // size in bytes: m_orderCount*sizeof(uint8_t)
    uint8_t *m_pOrders;

    // size in bytes: m_instrumentCount*sizeof(uint32_t)
    uint32_t *m_pInstruments;

    // size in bytes: m_sampleCount*sizeof(uint32_t)
    uint32_t *m_pSamples;

    // size in bytes: m_patternCount*sizeof(uint32_t)
    uint32_t *m_pPatterns;
    
    // fixed size 64-bytes
    uint16_t m_channelPanning[64];
    uint16_t m_channelVolume[64];

    uint8_t m_PitchWheelDepth; // MIDI-stuff
    uint8_t m_panningSeparation; // 0..128 
    
    uint8_t m_initialTempo;
    uint8_t m_initialSpeed;
    uint8_t m_mixVolume;
    uint8_t m_globalVolume;

    uint16_t m_special; // 
    uint16_t m_flags; // 
    
    uint16_t m_patternCount; // 
    uint16_t m_sampleCount; // 
    uint16_t m_instrumentCount; // 
    uint16_t m_orderCount; //
    
    // pattern row highlighting:    
    // only for editing, read anyway
    uint16_t m_patternRowHighlight;

    
    uint16_t m_Cmwt; // "Compatible with tracker", fileformat version?
    uint16_t m_Cwtv; // "Created with tracker" version?
    
    std::string m_songName;
    
public:
    CImpulseTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CImpulseTrackerPlayer();
    
    virtual bool ParseFileInfo();
    
    virtual DecodeCtx *PrepareDecoder();
    
    virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize);
};

#endif // IMPULSETRACKERPLAYER_H
