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

// entry in subsong list
struct AHXSubsongEntry_t
{
    // just a value?
    uint16_t m_song;
    
    // constructor
    AHXSubsongEntry_t()
    {
        m_song = 0;
    }
};

// channel in position list entry
struct AHXPositionChannel_t
{
    uint8_t m_track;
    int8_t m_transpose;
    
    // constructor
    AHXPositionChannel_t()
    {
        m_track = 0;
        m_transpose = 0;
    }
    
    void setFromArray(uint8_t *data)
    {
        m_track = data[0];
        m_transpose = data[1];
    }
};

// position list entry
struct AHXPositionEntry_t
{
    AHXPositionChannel_t m_channels[4];
    
    // constructor
    AHXPositionEntry_t()
    {}
    
    // keep values the eight bytes
    void setFromArray(uint8_t *data)
    {
        m_channels[0].setFromArray(data);
        m_channels[1].setFromArray(data + 2);
        m_channels[2].setFromArray(data + 4);
        m_channels[3].setFromArray(data + 6);
    }
};


// TRL-count of these on each track..
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

// single entry in playlist
struct AHXPlaylistEntry_t
{
    
    // constructor
    AHXPlaylistEntry_t()
    {
    }
};


// data of single sample
struct AHXSampleEntry_t
{
    // all values in sample..
    uint8_t m_data[22];
    
    // constructor
    AHXSampleEntry_t()
    {
        ::memset(m_data, 0, 22);
    }
    
    void setFromArray(uint8_t *data)
    {
        ::memcpy(m_data, data, 22);
    }
    
    uint8_t getPlaylistLen()
    {
        return m_data[21];
    }
};


class CAhxPlayer : public CModPlayer
{
protected:
    
    // this is used in which commands are available later
    enum AhxFormat
    {
        Unknown = 0,
        AHX0,
        AHX1
    };
    AhxFormat m_enAhxFormat;
    
    uint8_t m_trackSave;
    uint8_t m_playSpeed; // SPD (frequency multiplier for speed)
    uint16_t m_posListLen; // LEN, 1..999
    uint16_t m_restartPoint; // RES 0..(LEN-1)
    uint8_t m_trackLen; // TRL, 1..64
    uint8_t m_trackCount; // TRK, 0..255

    // count of samples
    uint8_t m_sampleCount; // SMP, 0..63

    // amount of subsongs
    uint8_t m_subsongCount; // SS, 0..255
    
    // see m_subsongCount
    AHXSubsongEntry_t *m_subsongList;
    
    // see m_posListLen
    AHXPositionEntry_t *m_positionList;

    // see m_trackCount and m_trackLen
    // also check m_trackSave (if TRK 0 was saved in file)
    AHXTrackEntry_t *m_trackListData;
    
    // 192 bytes of cleared memory for TRK 0,
    // see if this is needed..
    uint8_t *m_trackZero;

    // see m_sampleCount
    AHXSampleEntry_t *m_sampleData;
    
    
public:
    CAhxPlayer(CReadBuffer *pFileData);
    virtual ~CAhxPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // AHXPLAYER_H
