//////////////////////////////////////
//
// COktalyzerPlayer :
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

struct OKTPatternLine_t
{
    uint8_t newnote;
    uint8_t instrument; // sample
    uint8_t effect; 
    uint8_t data; // effect parameter
};

#pragma pack(pop)


class COktalyzerPlayer : public CModPlayer
{
protected:

    /*
    class OKTSampleData
    {
    public:
        OKTSampleData()
        {}
    };
    */
    
    // SBODY, upto 255 samples possible, usually limited to 36
    //OKTSampleData *m_pSampleBody;
    
    class OKTPatternData
    {
    public:
        OKTPatternData()
            : num_pattern_lines(0)
            , pattern_lines(nullptr)
        {}
        ~OKTPatternData()
        {
            delete [] pattern_lines;
        }
        
        // number of lines in this pattern
        int16_t num_pattern_lines;
        OKTPatternLine_t *pattern_lines;
    };

    // PBODY, amount of patterns in m_SongLength,
    // upto 64 patterns supported
    OKTPatternData *m_pPatternBody;
    
    // PATT chunk
    bufferedData_t m_PatternPositions;

    // PLEN chunk
    int16_t m_NumPatternPositions;
    
    // SLEN chunk
    int16_t m_SongLength;
    
    // SPEE chunk:
    // Vertical blank divisor for speed/tempo of playback ?
    int16_t m_AmigaVBLDivisor; /* InitialTempo */
    
    // SAMP chunk
    size_t m_nSampleCount;
    OKTSampleDirectory_t *m_pSampleData;
    
    // CMOD chunk
    uint16_t m_chan_flags[4];
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
public:
    COktalyzerPlayer(CReadBuffer *pFileData);
    virtual ~COktalyzerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // OKTALYZERPLAYER_H
