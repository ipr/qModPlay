//////////////////////////////////////
//
// CAhxPlayer :
// attempt at new (mostly) portable player
// for AHX0 and AHX1
//
// Based on documentation by: Stuart Caie
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef AHXPLAYER_H
#define AHXPLAYER_H

#include "ModPlayer.h"

// TRL-count of these on track format..
struct AHXTrackEntry_t
{
    // note: stored in three bytes,
    // do some bitshifting for simpler access..
    uint8_t m_note;
    uint8_t m_sample;
    uint8_t m_command;
    uint8_t m_commandData;
    
    // constructor
    AHXTrackEntry_t()
    {
        m_note = 0;
        m_sample = 0;
        m_command = 0;
        m_commandData = 0;
    }
    
    // parse values the three bytes:
    // this should simplify processing later.
    void setFromArray(uint8_t *data)
    {
        m_note = ((data[0] & 0xFC) >> 2); // first 6 bits
        
        // remaining two bits of first byte and
        // upper four bits from second byte
        m_sample = (((data[0] & 0x3) << 6) 
                | ((data[1] & 0xFC) >> 2);
        
        m_command = (data[1] & 0x0F); // lower 4 bits
        m_commandData = data[2]; // third byte as-is
    }
};

class CAhxPlayer : public CModPlayer
{
protected:
    
    uint8_t m_trackSave;
    uint8_t m_playSpeed; // SPD (frequency multiplier for speed)
    uint16_t m_posListLen; // LEN, 1..999
    uint16_t m_restartPoint; // RES 0..(LEN-1)
    uint8_t m_trackLen; // TRL, 1..64
    uint8_t m_trackCount; // TRK, 0..255

    // count of samples in mod
    uint8_t m_sampleCount; // SMP, 0..63

    // amount of subsongs in mod
    uint8_t m_subsongCount; // SS, 0..255
    
    // TODO: array of structs with these..
    // entry is 8 bytes with 4 sets (one for each audiochannel),
    // each set has track to play and transpose value (single bytes)
    uint8_t m_positionListEntry[8];
    
    // 192 bytes of cleared memory for TRK 0
    uint8_t *m_trackZero;
    
    // see m_trackCount and m_trackLen
    // also check m_trackSave (if TRK 0 was saved in file)
    AHXTrackEntry_t *m_trackListData;
    
public:
    CAhxPlayer(CReadBuffer *pFileData);
    virtual ~CAhxPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // AHXPLAYER_H
