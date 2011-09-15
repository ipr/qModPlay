//////////////////////////////////////
//
// CDigiBoosterProPlayer :
// player for DigiBooster PRO modules 
// in (mostly) portable C++
//
// Based on documentation by: Tomasz Piasta
//
// Official specification of DBM0 format:
// http://digibooster.eu/format.php
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef DIGIBOOSTERPROPLAYER_H
#define DIGIBOOSTERPROPLAYER_H

#include "ModPlayer.h"

#include <string>


// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

struct DBMInfoChunk_t
{
    uint16_t m_instruments;     // upto 255
    uint16_t m_samples;         // upto 255
    uint16_t m_songs;           // DBPro v2.x limited to 5
    uint16_t m_patterns;        // upto 1024
    uint16_t m_tracks;          // 4 to 254, even
};

struct DBMInstrumentChunk_t
{
    uint16_t m_sampleNumber;    // Number of sample used by this instrument
    uint16_t m_volume;          // Instrument default volume (from 0 to 64 including)
    uint32_t m_finetune;        // Sampling rate for note C-4 in Hz
    uint32_t m_repeatStart;     // The first looped sample
    uint32_t m_repeatLength;    // Length of loop in samples (0 for no loop)
    uint16_t m_generalPanning;  // Instrument panning (-128 = full left, +128 = full right)
    uint16_t m_flags;   // bit 0 = forward loop, bit 1 = ping-pong loop
};

// note: There is no reserved field before the first point, as DigiBooster 2.21 documentation mistakenly specifies
//
// same structure is used for both volume and panning envelopes
//
struct DBMEnvelope_t
{
    uint16_t m_instrumentNumber; // Number of instrument the envelope is attached to
    
    /* bits in flags:
        bit 0 = evenlope is turned on
        bit 1 = evenlope first sustain is active
        bit 2 = evenlope loop is active
        bit 3 = evenlope second sustain is active
    */
    uint8_t m_flags; // bitfield
    
    uint8_t m_pointCount; // Number of envelope points (up to 32)
    uint8_t m_firstSustainPoint; // Number of point of the first sustain
    uint8_t m_loopStartPoint; // Number of point of loop start
    uint8_t m_loopEndPoint; // Number of point of loop end
    uint8_t m_secondSustainPoint; // Number of point of the second sustain
    //uint8_t m_reserved;
    
    // note: rest of 136 bytes in this
    // are 16-bit integers in single array,
    // too lazy to add each one here..
};

#pragma pack(pop)


class CDigiBoosterProPlayer : public CModPlayer
{
protected:
    
    class DBMSong
    {
    public:
        DBMSong()
            : m_name()
            , m_playlistCount(0)
            , m_pOrders(nullptr)
        {}
        ~DBMSong()
        {
            delete m_pOrders;
        }
        
        std::string m_name;
        uint16_t m_playlistCount;
        uint16_t *m_pOrders;
    };

    class DBMInstrument
    {
    public:
        DBMInstrument()
            : m_name()
            , m_instruments()
        {}
        ~DBMInstrument()
        {
        }
        
        std::string m_name;
        DBMInstrumentChunk_t m_instruments;
    };

    class DBMPattern
    {
    public:
        DBMPattern()
            : m_rowCount(0)
            , m_patternData()
        {}
        ~DBMPattern()
        {
            delete m_patternData.m_pBuf;
        }
        
        uint16_t m_rowCount;
        bufferedData_t m_patternData;
    };

    class DBMSample
    {
    public:
        DBMSample()
            : m_flags(0)
            , m_sampleData()
        {}
        ~DBMSample()
        {
            delete m_sampleData.m_pBuf;
        }
        
        uint32_t m_flags;
        bufferedData_t m_sampleData;
    };

    class DBMEnvelope
    {
    public:
        DBMEnvelope()
            : m_envelope()
            , m_envelopeData()
        {}
        ~DBMEnvelope()
        {
            delete m_envelopeData.m_pBuf;
        }
        
        DBMEnvelope_t m_envelope;
        bufferedData_t m_envelopeData;
    };
    
    
    std::string m_moduleName;
    DBMInfoChunk_t m_moduleInfo;

    // amout equal to m_songs in DBMInfoChunk_t
    DBMSong *m_pSongBlocks;

    // amout equal to m_instruments in DBMInfoChunk_t
    DBMInstrument *m_pInstrBlocks;

    // amout equal to m_patterns in DBMInfoChunk_t
    DBMPattern *m_pPatternBlocks;

    // amout equal to m_samples in DBMInfoChunk_t
    DBMSample *m_pSampleBlocks;

    uint16_t m_volEnvelopeCount; // number of volume envelopes
    DBMEnvelope *m_pVolEnvelopes;

    uint16_t m_panEnvelopeCount; // number of panning envelopes
    DBMEnvelope *m_pPanEnvelopes;
    
    uint16_t m_patternNameEncoding;
    std::string *m_pPatternNames;
    
    uint8_t m_version;
    uint8_t m_revision;
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
    
public:
    CDigiBoosterProPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterProPlayer();
    
    virtual bool ParseFileInfo();

    virtual DecodeCtx *PrepareDecoder();
    
    // TODO:
    virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize);
};

#endif // DIGIBOOSTERPROPLAYER_H
