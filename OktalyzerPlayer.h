//////////////////////////////////////
//
// COktalyzerPlayer :
//
// just interface for now
//
// Based on documentation by: Harald Zappe
// (Date: Wed, 6 Apr 94 19:48:56 +0200)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef OKTALYZERPLAYER_H
#define OKTALYZERPLAYER_H

#include "ModPlayer.h"

#include <string>

// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

struct OKTSampleDirectory_t
{
    char Sample_Name[20];
    uint32_t Sample_Len;
    uint16_t Repeat_Start;
    uint16_t Repeat_Len;
    uint8_t pad1;
    uint8_t Volume;
    uint16_t pad2;
};



#pragma pack(pop)


class COktalyzerPlayer : public CModPlayer
{
protected:
    
    // PATT chunk
    bufferedData_t m_PatternPositions;

    // PLEN chunk
    int16_t m_NumPositions;
    
    // SLEN chunk
    int16_t m_SongLength;
    
    // SPEE chunk:
    // Vertical blank divisor for speed/tempo of playback ?
    int16_t m_AmigaVBLDivisor; /* InitialTempo */
    
    // CMOD chunk
    uint16_t m_chan_flags[4];
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
public:
    COktalyzerPlayer(CReadBuffer *pFileData);
    virtual ~COktalyzerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // OKTALYZERPLAYER_H
