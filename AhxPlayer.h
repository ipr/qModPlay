//////////////////////////////////////
//
// CAhxPlayer :
// attempt at new (mostly) portable player
// for AHX0 and AHX1
//
// Based on documentation by: Stuart Caie
//
// Note on implementation:
// could use bitfield-structs in some data
// but struct packing might be a problem with some compilers
// so I'd rather not do that and done in harder way..
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
        m_sample = ( ((data[0] & 0x3) << 4) 
                | ((data[1] & 0xFC) >> 2) );
        
        m_command = (data[1] & 0x0F); // lower 4 bits
        m_commandData = data[2]; // third byte as-is
    }
};

// single entry in playlist
struct AHXPlaylistEntry_t
{
    uint8_t m_fx2Command;
    uint8_t m_fx1Command;
    uint8_t m_waveForm;
    uint8_t m_fixNote;
    uint8_t m_noteData;
    uint8_t m_fx2Data;
    uint8_t m_fx1Data;
    
    // constructor
    AHXPlaylistEntry_t()
    {
        m_fx2Command = 0;
        m_fx1Command = 0;
        m_waveForm = 0;
        m_fixNote = 0;
        m_noteData = 0;
        m_fx2Data = 0;
        m_fx1Data = 0;
    }
    
    // values from four bytes
    void setFromArray(uint8_t *data)
    {
        // first three bits
        m_fx2Command = ((data[0] & 0xE0) >> 5); // first 3 bits
        m_fx1Command = ((data[0] & 0x1C) >> 2); // second 3 bits
        
        // remaining two bits of first byte and
        // uppermost bit from second byte
        m_waveForm = ( ((data[0] & 0x3) << 2)
                | ((data[1] & 0x80) >> 7) );

        // single bit
        m_fixNote = ((data[1] & 0x40) ? 1 : 0);
        m_noteData = (data[1] & 0x3F); // 6 bits
        
        m_fx2Data = data[2]; // third byte as-is
        m_fx1Data = data[3]; // fourth byte as-is
    }
};

// data of single sample
struct AHXSampleEntry_t
{
    // all values in sample..
    // TODO: write "open" or add accessors?
    uint8_t m_data[22];
    
    // playlist of sample, PLEN count
    // per-sample, right ?
    AHXPlaylistEntry_t *m_playlist;
    
    // constructor
    AHXSampleEntry_t()
    {
        ::memset(m_data, 0, 22);
    }
    // destructor
    ~AHXSampleEntry_t()
    {
        delete [] m_playlist;
    }
    
    void setFromArray(uint8_t *data)
    {
        ::memcpy(m_data, data, 22);
    }
    
    uint8_t getPlaylistLen()
    {
        // PLEN
        return m_data[21];
    }
    uint8_t getPlaylistDefaultSpeed()
    {
        return m_data[20];
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
    
    // if track zero is saved in file:
    // might not be if it is empty (use m_trackZero then),
    // otherwise read trackcount +1
    uint8_t m_trackZeroSaved;
    
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

    // whole size of track data..
    size_t m_trackListSize;

    // see m_trackCount and m_trackLen
    // also check m_trackSave (if TRK 0 was saved in file)
    AHXTrackEntry_t *m_trackListData;
    
    // 192 bytes of cleared memory for TRK 0,
    // see if this is needed..
    uint8_t *m_trackZero;

    // see m_sampleCount
    AHXSampleEntry_t *m_sampleData;
    
    // filename from metadata in file
    std::string m_fileName;
    
    // sample names (m_sampleCount)
    std::string *m_sampleNames;
    
public:
    CAhxPlayer(CReadBuffer *pFileData);
    virtual ~CAhxPlayer();
    
    virtual bool ParseFileInfo();
    
    // TODO: check parameters later..
    // prepare for playback
    virtual DecodeCtx *PrepareDecoder();
    
    // something like this to decode into buffer
    // for device-independent output in playback
    virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize);
};

#endif // AHXPLAYER_H
