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
    
public:
    CAhxPlayer(CReadBuffer *pFileData);
    virtual ~CAhxPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // AHXPLAYER_H
