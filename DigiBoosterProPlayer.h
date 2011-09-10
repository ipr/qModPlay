//////////////////////////////////////
//
// CDigiBoosterPlayer :
// rewrite of DigiBooster PRO player 
// in (mostly) portable C++
//
// Just interface for now..
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef DIGIBOOSTERPROPLAYER_H
#define DIGIBOOSTERPROPLAYER_H

#include "ModPlayer.h"

#include <string>


// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

struct DBMInfoChunk_t
{
    uint16_t m_instruments;
    uint16_t m_samples;
    uint16_t m_songs;
    uint16_t m_patterns;
    uint16_t m_channels;
};

struct DBMInstrumentChunk_t
{
    uint16_t m_sampleNumber;
    uint16_t m_volume;
    uint32_t m_finetune; // in Hz
    uint32_t m_repeatStart;
    uint32_t m_repeatLength;
    uint16_t m_generalPanning;
    uint16_t m_flags;
};

struct DBMVolEnvelope_t
{
    uint8_t m_type; // bitfield
    uint8_t m_pointCount;
    uint8_t m_sustainPoint1;
    uint8_t m_loopStartPoint;
    uint8_t m_loopEndPoint;
    uint8_t m_sustainPoint2;
    uint8_t m_reserved;
};


#pragma pack(pop)


class CDigiBoosterProPlayer : public CModPlayer
{
protected:
    
    std::string m_moduleName;

    std::string m_songName;
    uint16_t m_songOrderCount;
    uint16_t *m_pSongOrders;

    std::string m_instrumentName;
    
    DBMInfoChunk_t m_moduleInfo;
    DBMInstrumentChunk_t m_instruments;
    
    uint16_t m_patternRowCount;
    bufferedData_t m_patternData;

    uint32_t m_sampleFlags;
    bufferedData_t m_sampleData;

    uint16_t m_volEnvelopeCount;
    uint16_t m_instrumentNumber;
    
    DBMVolEnvelope_t m_volEnvelopeDesc;
    bufferedData_t m_volEnvelopeData;

    
    long m_version;
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
    
public:
    CDigiBoosterProPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterProPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPROPLAYER_H
