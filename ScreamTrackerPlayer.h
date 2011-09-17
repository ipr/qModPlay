//////////////////////////////////////
//
// CScreamTrackerPlayer :
//
// Based on documentation by unknown..
// (sorry, no name on the file..)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef SCREAMTRACKERPLAYER_H
#define SCREAMTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>

// unpacked format of pattern..
// note: not stored this way..
struct ST3Pattern_t
{
	// constructor
	ST3Pattern_t()
	{
		m_note = 0;
		m_instrument = 0;
		m_volume = 0;
		m_command = 0;
		m_info = 0;
	}
	
	uint8_t m_note;
	uint8_t m_instrument;
	uint8_t m_volume;
	uint8_t m_command;
	uint8_t m_info; // command info/data
};

class CScreamTrackerPlayer : public CModPlayer
{
protected:

    // just for memory aid if need becomes
    enum S3MFormatVersion
    {
        OLD_SIGNED_SAMPLES = 1,
        STANDARD_UNSIGNED_SAMPLES = 2
    };
	
	// for each channel, 64 patterns (rows)
	class ST3Channel
	{
	public:
		ST3Channel()
		{
		}
					   
		// 64 rows
		ST3Pattern_t m_patterns[64];
	};
	
	// for 32 channels
	ST3Channel *m_channels;
 
    // size in bytes: m_orderCount*sizeof(uint8_t)
    uint8_t *m_pOrders;
    
    // size in bytes: m_instrumentCount*sizeof(uint16_t)
	// (actually just fileoffsets in "parapointer" format)
    //uint16_t *m_pInstruments;

    // size in bytes: m_patternCount*sizeof(uint16_t)
	// (actually just fileoffsets in "parapointer" format)
    //uint16_t *m_pPatterns;
    
    // fixed size 32-bytes
    uint8_t m_channelSettings[32];

    uint8_t m_masterVolume;
    uint8_t m_initialTempo;
    uint8_t m_initialSpeed;
    uint8_t m_globalVolume;
    
    uint16_t m_patternCount; // 
    uint16_t m_instrumentCount; // 
    uint16_t m_orderCount; //

    uint16_t m_special; // 
    uint16_t m_flags; // 
    
    uint16_t m_Cwtv; // "Created with tracker" version?
    uint16_t m_Ffv; // Fileformat version?

    std::string m_songName;
    
	// see if this is correct..
	// should be simpler
	inline uint32_t ParaPtrToOffset(uint16_t para)
	{
		uint32_t offset = (para << 4);
		return offset;
	}
	
    // TODO: implement
    void ReadInstrumentData(size_t nOffset);
    void ReadPatternData(size_t nOffset);
    
public:
    CScreamTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CScreamTrackerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // SCREAMTRACKERPLAYER_H
