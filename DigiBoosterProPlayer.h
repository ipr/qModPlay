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
    
    std::string m_moduleName;
    DBMInfoChunk_t m_moduleInfo;

    // amout equal to m_songs in DBMInfoChunk_t
    DBMSong *m_pSongBlocks;

    std::string m_instrumentName;
    
    DBMInstrumentChunk_t m_instruments;
    
    uint16_t m_patternRowCount;
    bufferedData_t m_patternData;

    uint32_t m_sampleFlags;
    bufferedData_t m_sampleData;

    uint16_t m_volEnvelopeCount;
    uint16_t m_instrumentNumber;
    
    DBMVolEnvelope_t m_volEnvelopeDesc;
    bufferedData_t m_volEnvelopeData;

    
    uint8_t m_version;
    uint8_t m_revision;
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
    
public:
    CDigiBoosterProPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterProPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPROPLAYER_H
