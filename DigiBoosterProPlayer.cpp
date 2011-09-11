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

// for buffer-wrapper..
#include "AnsiFile.h"


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
        // (TODO: are there one or more instances of this chunk per file?)
        
        // instrument name
        m_instrumentName.assign((char*)m_pFileData->GetAtCurrent(), 44);
        m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 44);
        
        DBMInstrumentChunk_t *pInst = (DBMInstrumentChunk_t*)m_pFileData->GetAtCurrent();
        m_instruments.m_sampleNumber = Swap2(pInst->m_sampleNumber);
        m_instruments.m_volume = Swap2(pInst->m_volume);
        m_instruments.m_finetune = Swap4(pInst->m_finetune);
        m_instruments.m_repeatStart = Swap4(pInst->m_repeatStart);
        m_instruments.m_repeatLength = Swap4(pInst->m_repeatLength);
        m_instruments.m_generalPanning = Swap2(pInst->m_generalPanning);
        m_instruments.m_flags = Swap2(pInst->m_flags);
        
        return true;
    }
    else if (chunkID == IFFTag("PATT"))
    {
        // patterns hunk, variable length
        // (TODO: are there one or more instances of this chunk per file?)
        
        m_patternRowCount = Swap2(m_pFileData->NextUI2());
        
        // packed pattern: size and data
        m_patternData.m_nLen = Swap2(m_pFileData->NextUI2());
        m_patternData.m_pBuf = new uint8_t[m_patternData.m_nLen];
        ::memcpy(m_patternData.m_pBuf, m_pFileData->GetAtCurrent(), m_patternData.m_nLen);
        
        // debug
        //m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + m_patternData.m_nLen);
        return true;
    }
    else if (chunkID == IFFTag("SMPL"))
    {
        // samples hunk, variable length
        // (TODO: are there one or more instances of this chunk per file?)
        
        // flags:
        // - bit 0 set - 8 bit sample
        // - bit 1 set - 16 bit sample
        // - bit 2 set - 32 bit sample
        //
        m_sampleFlags = Swap4(m_pFileData->NextUI4());
        
        // sample data: size and data
        m_sampleData.m_nLen = Swap4(m_pFileData->NextUI4());
        m_sampleData.m_pBuf = new uint8_t[m_sampleData.m_nLen];
        ::memcpy(m_sampleData.m_pBuf, m_pFileData->GetAtCurrent(), m_sampleData.m_nLen);

        // - check flag for sample size
        // - swap byteorder for each value in buffer
        //
        uint32_t flag = 1;
        if (m_sampleFlags & (flag << 1))
        {
            // 16-bit
            uint16_t *pBuf = (uint16_t*)m_sampleData.m_pBuf;
            int count = m_sampleData.m_nLen / sizeof(uint16_t);
            for (int i = 0; i < count; i++)
            {
                pBuf[i] = Swap2(pBuf[i]);
            }
        }
        else if (m_sampleFlags & (flag << 2))
        {
            // 32-bit
            uint32_t *pBuf = (uint32_t*)m_sampleData.m_pBuf;
            int count = m_sampleData.m_nLen / sizeof(uint32_t);
            for (int i = 0; i < count; i++)
            {
                pBuf[i] = Swap4(pBuf[i]);
            }
        }
        
        // debug
        //m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + m_sampleData.m_nLen);
        return true;
    }
    else if (chunkID == IFFTag("VENV"))
    {
        // volume envelopes hunk (optional), variable length
        // (TODO: are there one or more instances of this chunk per file?)
        
        m_volEnvelopeCount = Swap2(m_pFileData->NextUI2());
        m_instrumentNumber = Swap2(m_pFileData->NextUI2());
        
        // volume envelope: fixed-size buffer? is it really?
        m_volEnvelopeData.m_nLen = 134;
        m_volEnvelopeData.m_pBuf = new uint8_t[m_volEnvelopeData.m_nLen];
        ::memcpy(m_volEnvelopeData.m_pBuf, m_pFileData->GetAtCurrent(), m_volEnvelopeData.m_nLen);
        
        // for simplicity.. do some parsing&byteswapping for later use
        ::memcpy(&m_volEnvelopeDesc, m_volEnvelopeData.m_pBuf, sizeof(DBMVolEnvelope_t));
        
        uint16_t *pEnvPts = (uint16_t*)(m_volEnvelopeData.m_pBuf + sizeof(DBMVolEnvelope_t));
        int count = (m_volEnvelopeData.m_nLen - sizeof(DBMVolEnvelope_t)) / sizeof(uint16_t);
        for (int i = 0; i < count; i++)
        {
            pEnvPts[i] = Swap2(pEnvPts[i]);
        }
        
        // debug
        //m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + m_volEnvelopeData.m_nLen);
        return true;
    }
    else if (chunkID == IFFTag("PENV"))
    {
        // panning envelopes (optional?), variable length
        // chunk added in newer Digibooster 3?
        
    }
    else if (chunkID == IFFTag("PNAM"))
    {
        // pattern names (optional?), variable length
        // chunk added in newer Digibooster 3?
        
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
    , m_instrumentName()
    , m_version(0)
{
}

CDigiBoosterProPlayer::~CDigiBoosterProPlayer() 
{
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
