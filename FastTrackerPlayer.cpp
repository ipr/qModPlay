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


#include "FastTrackerPlayer.h"


//////////// protected methods


// TODO: check needed parameters and where to call..
void CFastTrackerPlayer::OnPatternData()
{
    // TODO: can we just loop through each pattern?
    // (check storage)
    for (int i = 0; i < m_channelCount; i++)
    {
        // we should know channel count by now..
        m_patterns[i].m_packedSize = (m_channelCount*64);
        
        // TODO: check if this is empty and skip data if so (not stored)
        
        m_patterns[i].m_patternHeaderLen = ReadLEUI32();
        m_patterns[i].m_packingType = ReadUI8();
        m_patterns[i].m_rowCount = ReadLEUI16();
        m_patterns[i].m_packedSize = ReadLEUI16();
        
        // ..actual packed pattern data follows?
        
        // something like this? size in bytes or "count"?
        m_patterns[i] = new FT2PatternNote[m_patterns[i].m_packedSize];
        for (int j = 0; j < m_patterns[i].m_packedSize; j++)
        {
			uint8_t tmpNote = ReadUI8();
			
			// if highest bit is set, value is used for compression-information
			if (tmpNote & (1 << 8))
			{
				if (tmpNote & 1)
				{
					// bit 0 set: Note follows
					m_patterns[i].m_notes[j].m_note = ReadUI8();
				}
				if (tmpNote & (1 << 1))
				{
					// bit 1 set: Instrument follows
					m_patterns[i].m_notes[j].m_instrument = ReadUI8();
				}
				if (tmpNote & (1 << 2))
				{
					// bit 2 set: Volume column byte follows
					m_patterns[i].m_notes[j].m_volumeColumn = ReadUI8();
				}
				if (tmpNote & (1 << 3))
				{
					// bit 3 set: Effect type follows
					m_patterns[i].m_notes[j].m_effectType = ReadUI8();
				}
				if (tmpNote & (1 << 4))
				{
					// bit 4 set: Effect parameter follows
					m_patterns[i].m_notes[j].m_effectParameter = ReadUI8();
				}
			}
			else
			{
				// otherwise we use values "as-is" ??? 
				// or what? just note given no other information?
				m_patterns[i].m_notes[j].m_note = tmpNote;
			}
		}
    }
}

void CFastTrackerPlayer::OnInstruments()
{
    for (int i = 0; i < m_instrumentCount; i++)
    {
		// temp.. should keep somewhere too..
		//
        uint32_t instrSize = ReadLEUI32();
        std::string name;
        name.assign(m_pFileData->GetNext(22), 22);
        uint8_t type = ReadUI8();
        uint16_t sampleCount = ReadLEUI16();
        if (sampleCount > 0)
        {
			// descriptor of instrument and related sample
			FT2InstrumentSample_t *pInstrDesc = (FT2InstrumentSample_t*)m_pFileData->GetNext(sizeof(FT2InstrumentSample_t));
			// TODO: keep values somewhere..
			
			// header of actual sample
			FT2SampleHeader_t *pSampleHdr = (FT2SampleHeader_t*)m_pFileData->GetNext(sizeof(FT2SampleHeader_t));
			
			// delta-packed (signed) sample-data follows..
			// unpack and keep somewhere
			//
			//DeltaUnpack(m_pFileData->GetNext(pSampleHdr->m_sampleLength));
			//
			// something like this instead?
			//
			CAudioSample *pSample = new CAudioSample();
			pSample->fromDeltaPacked8bit(m_pFileData->GetNext(pSampleHdr->m_sampleLength), pSampleHdr->m_sampleLength);
			pSample->setValueType(CAudioSample::VT_SIGNED_INT); // signed int
			pSample->setWidth(8); // 8-bits
        }
    }
}


/////////// public


CFastTrackerPlayer::CFastTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_patternOrders()
{
}

CFastTrackerPlayer::~CFastTrackerPlayer()
{
    delete m_patternOrders.m_pBuf;
}

bool CFastTrackerPlayer::ParseFileInfo()
{
	// quick&easy way to check..
	if (::memcmp(m_pFileData->GetNext(17), "Extended module: ", 17) != 0
		&& ::memcmp(m_pFileData->GetNext(17), "Extended Module: ", 17) != 0)
    {
        // unsupported format
        return false;
    }
    
    // mod name (padded with zeroes)
    m_moduleName.assign((char*)m_pFileData->GetNext(20), 20);
    m_mystery = ReadUI8(); // $1a
    m_trackerName.assign((char*)m_pFileData->GetNext(20), 20);
    
    // keep individual bytes..
    m_version = ReadUI8();
    m_revision = ReadUI8();
    
    // no need to keep, just for debugging now..
    m_headerSize = ReadLEUI32(); 
    
    m_songLength = ReadLEUI16();
    m_restartPosition = ReadLEUI16();
    m_channelCount = ReadLEUI16();
    m_patternCount = ReadLEUI16();
    m_instrumentCount = ReadLEUI16();
    m_flags = ReadLEUI16();
    m_defaultTempo = ReadLEUI16();
    m_defaultBPM = ReadLEUI16();

	// we could skip this buffer and just process these directly..
	//OnPatternData();
	// for now, just read it..	
    m_patternOrders.m_nLen = 256; // fixed 
    m_patternOrders.m_pBuf = new uint8_t[m_patternOrders.m_nLen];
    m_pFileData->NextArray(m_patternOrders.m_pBuf, m_patternOrders.m_nLen);
    
    // TODO: this is wrong? (should be by patterncount instead of channelcount..?)
    // allocate area for pattern data, including empty patterns (which aren't saved):
    // size amount of channels, each pattern at least 64 bytes
    //m_patterns = new FT2Pattern[m_channelCount];
    
    
    return true;
}
