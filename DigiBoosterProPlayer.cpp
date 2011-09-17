//////////////////////////////////////
//
// CDigiBoosterProPlayer :
// player for DigiBooster PRO modules 
// in (mostly) portable C++
//
// Based on documentation by: Tomasz Piasta
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "DigiBoosterProPlayer.h"


//////////// protected methods

bool CDigiBoosterProPlayer::OnChunk(uint32_t chunkID, const uint32_t chunkLen)
{
    if (chunkID == IFFTag("NAME"))
    {
        // fixed length: 44 characters for module name
        m_moduleName.assign((char*)m_pFileData->GetAtCurrent(), 44);
        return true;
    }
    else if (chunkID == IFFTag("INFO"))
    {
        // module info chunk, fixed size: 10 bytes
        
        DBMInfoChunk_t *pInfo = (DBMInfoChunk_t*)m_pFileData->GetAtCurrent();
        m_moduleInfo.m_instruments = Swap2(pInfo->m_instruments);
        m_moduleInfo.m_samples = Swap2(pInfo->m_samples);
        m_moduleInfo.m_songs = Swap2(pInfo->m_songs);
        m_moduleInfo.m_patterns = Swap2(pInfo->m_patterns);
        m_moduleInfo.m_tracks = Swap2(pInfo->m_tracks);
        return true;
    }
    else if (chunkID == IFFTag("SONG"))
    {
        // songs hunk, variable length
        
        m_pSongBlocks = new DBMSong[m_moduleInfo.m_songs];

        for (int song = 0; song < m_moduleInfo.m_songs; song++)
        {
            // song name
            m_pSongBlocks[song].m_name.assign((char*)m_pFileData->GetAtCurrent(), 44);
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 44);
            
            // song orders
            m_pSongBlocks[song].m_playlistCount = Swap2(m_pFileData->NextUI2());
            m_pSongBlocks[song].m_pOrders = new uint16_t[m_pSongBlocks[song].m_playlistCount];
            for (int i = 0; i < m_pSongBlocks[song].m_playlistCount; i++)
            {
                m_pSongBlocks[song].m_pOrders[i] = Swap2(m_pFileData->NextUI2());
            }
        }
        return true;
    }
    else if (chunkID == IFFTag("INST"))
    {
        // instruments hunk, variable length
        
        m_pInstrBlocks = new DBMInstrument[m_moduleInfo.m_instruments];
        
        for (int instr = 0; instr < m_moduleInfo.m_instruments; instr++)
        {
            // instrument name
            m_pInstrBlocks[instr].m_name.assign((char*)m_pFileData->GetAtCurrent(), 44);
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 44);
            
            DBMInstrumentChunk_t *pInst = (DBMInstrumentChunk_t*)m_pFileData->GetAtCurrent();
            m_pInstrBlocks[instr].m_instruments.m_sampleNumber = Swap2(pInst->m_sampleNumber);
            m_pInstrBlocks[instr].m_instruments.m_volume = Swap2(pInst->m_volume);
            m_pInstrBlocks[instr].m_instruments.m_finetune = Swap4(pInst->m_finetune);
            m_pInstrBlocks[instr].m_instruments.m_repeatStart = Swap4(pInst->m_repeatStart);
            m_pInstrBlocks[instr].m_instruments.m_repeatLength = Swap4(pInst->m_repeatLength);
            m_pInstrBlocks[instr].m_instruments.m_generalPanning = Swap2(pInst->m_generalPanning);
            m_pInstrBlocks[instr].m_instruments.m_flags = Swap2(pInst->m_flags);
        }
        return true;
    }
    else if (chunkID == IFFTag("PATT"))
    {
        // patterns hunk, variable length

        m_pPatternBlocks = new DBMPattern[m_moduleInfo.m_patterns];
        
        for (int patt = 0; patt < m_moduleInfo.m_patterns; patt++)
        {
            m_pPatternBlocks[patt].m_rowCount = Swap2(m_pFileData->NextUI2());
            
            // packed pattern: size and data
            m_pPatternBlocks[patt].m_patternData.m_nLen = Swap2(m_pFileData->NextUI2());
            m_pPatternBlocks[patt].m_patternData.m_pBuf = new uint8_t[m_pPatternBlocks[patt].m_patternData.m_nLen];
            
            // copy packed pattern data
            m_pFileData->NextArray(m_pPatternBlocks[patt].m_patternData.m_pBuf, m_pPatternBlocks[patt].m_patternData.m_nLen);
            
            // TODO: unpacking while playing?
        }
        return true;
    }
    else if (chunkID == IFFTag("SMPL"))
    {
        // samples hunk, variable length
        // note: sample data actually signed integers, take note on playback
        
        m_pSampleBlocks = new DBMSample[m_moduleInfo.m_samples];
        for (int smpl = 0; smpl < m_moduleInfo.m_samples; smpl++)
        {
            // flags:
            // - bit 0 set - 8 bit sample
            // - bit 1 set - 16 bit sample
            // - bit 2 set - 32 bit sample
            //
            m_pSampleBlocks[smpl].m_flags = Swap4(m_pFileData->NextUI4());
            
            // sample data: size and data
            m_pSampleBlocks[smpl].m_sampleData.m_nLen = Swap4(m_pFileData->NextUI4());
            m_pSampleBlocks[smpl].m_sampleData.m_pBuf = new uint8_t[m_pSampleBlocks[smpl].m_sampleData.m_nLen];
            ::memcpy(m_pSampleBlocks[smpl].m_sampleData.m_pBuf, m_pFileData->GetAtCurrent(), m_pSampleBlocks[smpl].m_sampleData.m_nLen);
    
            // - check flag for sample size
            // - swap byteorder for each value in buffer
            //
            uint32_t flag = 1;
            if (m_pSampleBlocks[smpl].m_flags & (flag << 1))
            {
                // 16-bit
                uint16_t *pBuf = (uint16_t*)m_pSampleBlocks[smpl].m_sampleData.m_pBuf;
                int count = m_pSampleBlocks[smpl].m_sampleData.m_nLen / sizeof(uint16_t);
                for (int i = 0; i < count; i++)
                {
                    pBuf[i] = Swap2(pBuf[i]);
                }
            }
            else if (m_pSampleBlocks[smpl].m_flags & (flag << 2))
            {
                // 32-bit
                uint32_t *pBuf = (uint32_t*)m_pSampleBlocks[smpl].m_sampleData.m_pBuf;
                int count = m_pSampleBlocks[smpl].m_sampleData.m_nLen / sizeof(uint32_t);
                for (int i = 0; i < count; i++)
                {
                    pBuf[i] = Swap4(pBuf[i]);
                }
            }
        }
        return true;
    }
    else if (chunkID == IFFTag("VENV"))
    {
        // volume envelopes hunk (optional), variable length,
        // contains n amount of fixed-size blocks
        
        m_volEnvelopeCount = Swap2(m_pFileData->NextUI2());
        
        m_pVolEnvelopes = new DBMEnvelope[m_volEnvelopeCount];
        for (int env = 0; env < m_volEnvelopeCount; env++)
        {
            // volume envelope is fixed size 
            
            DBMEnvelope_t *pVEnv = (DBMEnvelope_t*)m_pFileData->GetAtCurrent();
            ::memcpy(&(m_pVolEnvelopes[env].m_envelope), pVEnv, sizeof(DBMEnvelope_t));
            
            m_pVolEnvelopes[env].m_envelope.m_instrumentNumber = Swap2(m_pVolEnvelopes[env].m_envelope.m_instrumentNumber);
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + sizeof(DBMEnvelope_t));
            
            // rest of data..
            m_pVolEnvelopes[env].m_envelopeData.m_nLen = (136 - sizeof(DBMEnvelope_t));
            m_pVolEnvelopes[env].m_envelopeData.m_pBuf = new uint8_t[m_pVolEnvelopes[env].m_envelopeData.m_nLen];
            m_pFileData->NextArray(m_pVolEnvelopes[env].m_envelopeData.m_pBuf, m_pVolEnvelopes[env].m_envelopeData.m_nLen);

            // for simplicity.. do some parsing&byteswapping for later use
            uint16_t *pEnvPts = (uint16_t*)(m_pVolEnvelopes[env].m_envelopeData.m_pBuf);
            int count = (m_pVolEnvelopes[env].m_envelopeData.m_nLen / sizeof(uint16_t));
            for (int i = 0; i < count; i++)
            {
                pEnvPts[i] = Swap2(pEnvPts[i]);
            }
        }
        return true;
    }
    else if (chunkID == IFFTag("PENV"))
    {
        // panning envelopes hunk (optional), variable length,
        // contains n amount of fixed-size blocks
        
        m_panEnvelopeCount = Swap2(m_pFileData->NextUI2());
        
        m_pPanEnvelopes = new DBMEnvelope[m_panEnvelopeCount];
        for (int env = 0; env < m_panEnvelopeCount; env++)
        {
            // volume envelope is fixed size 
            
            DBMEnvelope_t *pPEnv = (DBMEnvelope_t*)m_pFileData->GetAtCurrent();
            ::memcpy(&(m_pPanEnvelopes[env].m_envelope), pPEnv, sizeof(DBMEnvelope_t));
            
            m_pPanEnvelopes[env].m_envelope.m_instrumentNumber = Swap2(m_pPanEnvelopes[env].m_envelope.m_instrumentNumber);
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + sizeof(DBMEnvelope_t));
            
            // rest of data..
            m_pPanEnvelopes[env].m_envelopeData.m_nLen = (136 - sizeof(DBMEnvelope_t));
            m_pPanEnvelopes[env].m_envelopeData.m_pBuf = new uint8_t[m_pPanEnvelopes[env].m_envelopeData.m_nLen];
            m_pFileData->NextArray(m_pPanEnvelopes[env].m_envelopeData.m_pBuf, m_pPanEnvelopes[env].m_envelopeData.m_nLen);

            // for simplicity.. do some parsing&byteswapping for later use
            uint16_t *pEnvPts = (uint16_t*)(m_pPanEnvelopes[env].m_envelopeData.m_pBuf);
            int count = (m_pPanEnvelopes[env].m_envelopeData.m_nLen / sizeof(uint16_t));
            for (int i = 0; i < count; i++)
            {
                pEnvPts[i] = Swap2(pEnvPts[i]);
            }
        }
        return true;
    }
    else if (chunkID == IFFTag("PNAM"))
    {
        // pattern names (optional?), variable length
        // chunk added in newer Digibooster 3?

        // encoding used in names:
        // 0    = 8-bit ASCII
        // 106  = UTF-8
        //
        m_patternNameEncoding = Swap2(m_pFileData->NextUI2());

        // TODO: get QTextCodec for codepage conversion..
        
        m_pPatternNames = new std::string[m_moduleInfo.m_patterns];
        for (int i = 0; i < m_moduleInfo.m_patterns; i++)
        {
            uint8_t len = m_pFileData->NextUI1();
            m_pPatternNames[i].assign((char*)m_pFileData->GetAtCurrent(), len);
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + len);
        }
        return true;
    }
    
    // TODO: debug output: unsupport chunk found..
    return false;
}


/////////// public


CDigiBoosterProPlayer::CDigiBoosterProPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_moduleName()
    , m_moduleInfo()
    , m_pSongBlocks(nullptr)
    , m_pInstrBlocks(nullptr)
    , m_pPatternBlocks(nullptr)
    , m_pSampleBlocks(nullptr)
    , m_pVolEnvelopes(nullptr)
    , m_pPanEnvelopes(nullptr)
    , m_patternNameEncoding(0)
    , m_pPatternNames(nullptr)
    , m_version(0)
{
}

CDigiBoosterProPlayer::~CDigiBoosterProPlayer() 
{
    delete [] m_pPatternNames;
    delete [] m_pPanEnvelopes;
    delete [] m_pVolEnvelopes;
    delete [] m_pSampleBlocks;
    delete [] m_pPatternBlocks;
    delete [] m_pInstrBlocks;
    delete [] m_pSongBlocks;
}

/*
 notes on file format..
 
 pretty much like IFF-standard format,
 chunkID with chunkLen given in each chunk

 can reuse much code from other programs
 
 file-header chunk has minor difference
 to IFF so that's handled here
*/

bool CDigiBoosterProPlayer::ParseFileInfo()
{
    if (m_pFileData->NextUI4() != IFFTag("DBM0"))
    {
        // not supported module
        return false;
    }
    
    // version and revision (as BCD value?)
    // -> handle ..
    m_version = m_pFileData->NextUI1();
    m_revision = m_pFileData->NextUI1();
    
    // skip two bytes (reserved)
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos()+2);

    while (m_pFileData->IsEnd() == false)
    {
        // common IFF-tag style chunk ID, no need for byteswap
        uint32_t chunkID = m_pFileData->NextUI4();
        
        // length always given
        uint32_t chunkLen = Swap4(m_pFileData->NextUI4());

        // keep chunk start position
        size_t nPos = m_pFileData->GetCurrentPos();
        
        if (OnChunk(chunkID, chunkLen) == false)
        {
        }

        // we know size -> just go to next one
        // (can skip if chunk is not handled)
        m_pFileData->SetCurrentPos(nPos + chunkLen);
        
        // no more data (no chunkID+chunkLen)
        if ((m_pFileData->GetSize() - m_pFileData->GetCurrentPos()) < 8)
        {
            break;
        }
    }
    
    return true;
}

DecodeCtx *CDigiBoosterProPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
	//m_pDecodeCtx->initialize();
    
    return m_pDecodeCtx;
}


// TODO:
size_t CDigiBoosterProPlayer::DecodePlay(void *pBuffer, const size_t nBufSize)
{
	// get last position before next..
	uint64_t frame = m_pDecodeCtx->position();
	double duration = m_pDecodeCtx->frameduration();
	size_t frameSize = m_pDecodeCtx->frameSize();
	
	// current time in playback:
	// see pattern/track speed/tempo,
	// keep track of sample/effect length etc.. timing stuff
	//
	double frameTime = frame*duration;
	
	// count amount of whole frames fitting to given buffer
	size_t outFrames = (nBufSize/frameSize);


	// write to buffer as much as there fits
	for (size_t n = 0; n < outFrames; n++)
	{
		// TODO: count correct pattern where we continue
		// since we can't write entire song at single time to output..
		//DBMPattern *pPatt = m_pPatternBlocks[0];
		
		
	}
	
	// keep which frame we finished on
	m_pDecodeCtx->updatePosition(frame + outFrames);
	
	// return bytes written to buffer:
	// same amount will be written to audiodevice
    return (outFrames*frameSize);
}

