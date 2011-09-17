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


#include "ScreamTrackerPlayer.h"


//////////// protected methods

// TODO: implement
void CScreamTrackerPlayer::ReadInstrumentData(size_t nOffset)
{
	// parse&convert?
	//CAudioSample *pSample = CAdlibSample::ParseSample(m_pFileData->GetAt(nOffset), ??);
}

// TODO: read packed pattern
void CScreamTrackerPlayer::ReadPatternData(size_t nOffset, int index)
{
	// start at given offset
	m_pFileData->SetCurrentPos(nOffset);
	
	// unpack to this format..
	//ST3Pattern_t pattern;

	// TODO: somekinda current pattern counter..
	//size_t nrow; // = ??;
	
	uint16_t patternLen = ReadLEUI16();
	
	size_t nPos = m_pFileData->GetCurrentPos();
	
	// loop until read entirely..
	//
	while ((nPos-nOffset) < patternLen)
	{
		uint8_t packbyte = ReadUI8();
		if (packbyte == 0)
		{
			// this pattern is done?
			// (each 64 rows..)
			// -> set empty to each channel?
			/*
			for (int i = 0; i < 32; i++)
			{
				//m_channels[i].m_patterns[nrow]
			}
			*/
			return;
		}
		
		int channel = (packbyte & 31);
		ST3Channel *pChannel = &(m_channels[channel]); 
		
		if ((packbyte & 32) != 0)
		{
			pChannel->m_patterns[index].m_note = ReadUI8();
			pChannel->m_patterns[index].m_instrument = ReadUI8();
		}
		if ((packbyte & 64) != 0)
		{
			pChannel->m_patterns[index].m_volume = ReadUI8();
		}
		if ((packbyte & 128) != 0)
		{
			pChannel->m_patterns[index].m_command = ReadUI8();
			pChannel->m_patterns[index].m_info = ReadUI8();
		}
	}
	
}


/////////// public

CScreamTrackerPlayer::CScreamTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pOrders(nullptr)
    //, m_pInstruments(nullptr)
    //, m_pPatterns(nullptr)
	, m_channels(nullptr)
{
}

CScreamTrackerPlayer::~CScreamTrackerPlayer()
{
	delete [] m_channels;
    //delete m_pPatterns;
    //delete m_pInstruments;
    delete m_pOrders;
}

bool CScreamTrackerPlayer::ParseFileInfo()
{
    if (::memcmp(m_pFileData->GetAt(44), "SCRM", 4) != 0)
    {
        // unsupported format/unknown identifier
        return false;
    }
    
    // song name, fixed length with NULL
    m_songName.assign(m_pFileData->GetBegin(), 28);
    m_pFileData->SetCurrentPos(28);

    // actually, skip some four bytes (check later..)
    // (1Ah, Typ, x, x)
    m_pFileData->SetCurrentPos(0x20);
    
    m_orderCount = ReadLEUI16(); 
    m_instrumentCount = ReadLEUI16(); 
    m_patternCount = ReadLEUI16(); 

    m_flags = ReadLEUI16();
    m_Cwtv = ReadLEUI16();
    m_Ffv = ReadLEUI16();
    
    if (m_Ffv != OLD_SIGNED_SAMPLES 
        && m_Ffv != STANDARD_UNSIGNED_SAMPLES)
    {
        // file format version unsupported here,
        // currently anyway..
        return false;
    }
    
    // skip format identifier (checked at beginning)
    m_pFileData->SetCurrentPos(0x30);
    
    m_globalVolume = ReadUI8();
    m_initialSpeed = ReadUI8();
    m_initialTempo = ReadUI8();
    m_masterVolume = ReadUI8();
    
    // skip some..
    m_pFileData->SetCurrentPos(0x3E);
    
    m_special = ReadLEUI16(); // custom data offset
    
    // 32 bytes: channel settings for 32 channels
    m_pFileData->NextArray(m_channelSettings, 32);

    size_t nSize = m_orderCount*sizeof(uint8_t);
    if (nSize % 2 != 0)
    {
        // padding needed? "should be even" ?
        nSize += 1;
    }
    m_pOrders = new uint8_t[nSize];
    m_pFileData->NextArray(m_pOrders, nSize);

    // count header end position:
    // add size of "parapointers" (n*sizeof(uint16_t))
    size_t nHeaderEnd = m_pFileData->GetCurrentPos();
    nHeaderEnd += (m_instrumentCount*sizeof(uint16_t));
    nHeaderEnd += (m_patternCount*sizeof(uint16_t));

    // actually, "parapointers" which are just weird offsets to file..
	// 16-bit PC crap?

    // handle "parapointers" for instruments
    for (int i = 0; i < m_instrumentCount; i++)
    {
        // read instruments offset
        uint32_t offset = ParaPtrToOffset(ReadLEUI16());

		size_n nPos = m_pFileData->GetCurrentPos();
		
		// read instument at given offset
        ReadInstrumentData(offset);
		m_pFileData->SetCurrentPos(nPos); // continue this list
    }

	// for playback, always 32 channels ?
	m_channels = new ST3Channel[32];
	
    // actually, "parapointers" which are just weird offsets to file..
	// 16-bit PC crap?
    
    // handle "parapointers" for patterns
    for (int i = 0; i < m_patternCount; i++)
    {
        // read pattern offset
		uint32_t offset = ParaPtrToOffset(ReadLEUI16());

		size_n nPos = m_pFileData->GetCurrentPos();
		
		// read pattern at given offset
        ReadPatternData(offset, i);
		m_pFileData->SetCurrentPos(nPos); // continue this list
    }
    
    
    // TODO: sample-handling etc.
    
    
    return true;
}

DecodeCtx *CScreamTrackerPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
    
    return m_pDecodeCtx;
}
