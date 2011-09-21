//////////////////////////////////////
//
// CFastTrackerPlayer : FastTracker 2 format files
// note: should rename this class..
// This one handles XM format of FT2
// instead of MOD-like format of first FastTracker..
//
// Based on documentation by: Fredrik Huss / Mr.H of Triton in 1994
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef FASTTRACKERPLAYER_H
#define FASTTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>

// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

struct FT2InstrumentSample_t
{
	uint32_t m_headerSize;
	uint8_t m_sampleNumbers[96];
	uint8_t m_volEnvelopePoints[48];
	uint8_t m_panEnvelopePoints[48];
	
	uint8_t m_volumePointCount;
	uint8_t m_panningPointCount;
	
	uint8_t m_volumeSustainPoint;
	uint8_t m_volumeLoopStartPoint;
	uint8_t m_volumeLoopEndPoint;
	
	uint8_t m_panningSustainPoint;
	uint8_t m_panningLoopStartPoint;
	uint8_t m_panningLoopEndPoint;
	
	uint8_t m_volumeType; // bit 0: On; 1: Sustain; 2: Loop
	uint8_t m_panningType; // bit 0: On; 1: Sustain; 2: Loop
	
	uint8_t m_vibratoType; 
	uint8_t m_vibratoSweep; 
	uint8_t m_vibratoDepth; 
	uint8_t m_vibratoRate; 
	
	uint16_t m_volumeFadeOut;
	uint16_t reserved; //
};

struct FT2SampleHeader_t
{
	uint32_t m_sampleLength;
	uint32_t m_sampleLoopStart;
	uint32_t m_sampleLoopLength;

	uint8_t m_volume; 
	int8_t m_finetune; 
	uint8_t m_type; 
	uint8_t m_panning; 
	
	int8_t m_relativeNoteNumber; 
	uint8_t m_reserved; 
	
	char m_sampleName[22];
};


#pragma pack(pop)


// TODO: should be CFastTracker2Player.. old is more protracker-like file..
class CFastTrackerPlayer : public CModPlayer
{
protected:

	struct FT2PatternNote
	{
		uint8_t m_note;
		uint8_t m_instrument;
		uint8_t m_volumeColumn;
		uint8_t m_effectType;
		uint8_t m_effectParameter;

		// constructor
		FT2PatternNote()
		{
			// slightly unsure of packed-pattern format so initialize properly..
			m_note = 0;
			m_instrument = 0;
			m_volumeColumn = 0;
			m_effectType = 0;
			m_effectParameter = 0;
		}
	};
    
    // empty patterns are not saved in file
    // so we always need to allocate
    // enough in case empty pattern is referenced somehow..
    class FT2Pattern
    {
    public:
        FT2Pattern()
            : m_notes(nullptr)
            //, m_patternData()
            , m_patternHeaderLen(9)
            , m_packingType(0)
            , m_rowCount(64)
            , m_packedSize(0) // actually, 64*chcount, but we don't know that yet..
        {
	        /*
            m_patternData.m_nLen = m_rowCount; // 64 bytes
            m_patternData.m_pBuf = uint8_t[m_patternData.m_nLen];
            ::memset(m_patternData.m_pBuf, 0x80, m_patternData.m_nLen);
            */
        }
        ~FT2Pattern()
        {
            //delete m_patternData.m_pBuf;
            delete [] m_notes;
        }
        
        // pre-allocate in case will be empty..
        // size: 64 bytes
        // initial value $80 (in case empty)
        //bufferedData_t m_patternData;
        // replace with this?
        FT2PatternNote *m_notes;
        
        uint32_t m_patternHeaderLen;
        uint8_t m_packingType; // always 0 ?
        uint16_t m_rowCount;
        uint16_t m_packedSize;
        
    };
    // size: number of channels
    // TODO: is this wrong? 
    FT2Pattern *m_patterns;
    
	// fixed 256 bytes
    bufferedData_t m_patternOrders;
    
    uint16_t m_songLength;
    uint16_t m_restartPosition;
    uint16_t m_channelCount;
    uint16_t m_patternCount; // max. 256
    uint16_t m_instrumentCount; // max. 128
    uint16_t m_flags; // 0 = Amiga frequency table, 1 = linear frequency table
    uint16_t m_defaultTempo;
    uint16_t m_defaultBPM;
    
    // no need to keep, just for debugging now..
    uint32_t m_headerSize;
    
    uint8_t m_mystery; // $1a
    
    //uint16_t m_version;
    uint8_t m_version;
    uint8_t m_revision;
    
    std::string m_moduleName;
    std::string m_trackerName;
    
    // TODO: check needed parameters..
    void OnPatternData();
    void OnInstruments();
    
public:
    CFastTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CFastTrackerPlayer();

    virtual bool ParseFileInfo();
};

#endif // FASTTRACKERPLAYER_H
