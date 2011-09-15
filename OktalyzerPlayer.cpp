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


#include "OktalyzerPlayer.h"


//////////// protected methods

/*
 period table:
 same as used for SoundTracker, ProTracker, NoiseTracker etc. modules
 
 direct copy from documentation..
*/
static short pertab[] =
{
/*  C    C#     D    D#      E     F    F#     G    G#      A    A#     B */
 0x358,0x328,0x2FA,0x2D0, 0x2A6,0x280,0x25C,0x23A, 0x21A,0x1FC,0x1E0,0x1C5,
 0x1AC,0x194,0x17D,0x168, 0x153,0x140,0x12E,0x11D, 0x10D, 0xFE, 0xF0, 0xE2,
  0xD6, 0xCA, 0xBE, 0xB4,  0xAA, 0xA0, 0x97, 0x8F,  0x87, 0x7F, 0x78, 0x71
};


bool COktalyzerPlayer::OnChunk(uint32_t chunkID, const uint32_t chunkLen)
{
    if (chunkID == IFFTag("CMOD"))
    {
        // channel modes
        for (int i = 0; i < 4; i++)
        {
            m_chan_flags[i] = Swap2(m_pFileData->NextUI2());
        }
        return true;
    }
    else if (chunkID == IFFTag("SAMP"))
    {
        // sample directories
        
        m_nSampleCount = (chunkLen / sizeof(OKTSampleDirectory_t));
        m_pSampleData = new OKTSampleDirectory_t[m_nSampleCount];
        for (int i = 0; i < m_nSampleCount; i++)
        {
            // byteswap & keep values
            
            OKTSampleDirectory_t *pSampleDir = (OKTSampleDirectory_t*)m_pFileData->GetAtCurrent();
            
            ::memcpy(m_pSampleData[i].Sample_Name, pSampleDir->Sample_Name, 20);
            m_pSampleData[i].Sample_Len = Swap4(pSampleDir->Sample_Len);
            m_pSampleData[i].Repeat_Start = Swap2(pSampleDir->Repeat_Start);
            m_pSampleData[i].Repeat_Len = Swap2(pSampleDir->Repeat_Len);
            m_pSampleData[i].Volume = pSampleDir->Volume;
            
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + sizeof(OKTSampleDirectory_t));
        }
        return true;
    }
    else if (chunkID == IFFTag("SPEE"))
    {
        // speed (initial tempo)
        // Vertical blank divisor for speed/tempo of playback ?
        //
        m_AmigaVBLDivisor = Swap2(m_pFileData->NextUI2());
        return true;
    }
    else if (chunkID == IFFTag("SLEN"))
    {
        // song length: count of patterns in module,
        // use same for number of "PBODY" chunks
        //
        m_SongLength = Swap2(m_pFileData->NextUI2());
        
        //m_pPatternBody = new OKTPatternData[m_SongLength];
        return true;
    }
    else if (chunkID == IFFTag("PLEN"))
    {
        // number of pattern positions (see "PATT" chunk)
        m_NumPatternPositions = Swap2(m_pFileData->NextUI2());
        return true;
    }
    else if (chunkID == IFFTag("PATT"))
    {
        // pattern positions
        // seems to be fixed length chunk?
        // just byte* position
        
        m_PatternPositions.m_nLen = chunkLen;
        m_PatternPositions.m_pBuf = new uint8_t[m_PatternPositions.m_nLen];
        m_pFileData->NextArray(m_PatternPositions.m_pBuf, m_PatternPositions.m_nLen);
        
        return true;
    }
    else if (chunkID == IFFTag("PBOD"))
    {
        // TODO: index next body-chunk
        // or replace with nodelist..?
        size_t n; // = ??
        
        // pattern body 1..n (multiple in single module),
        // multiple lines (one for each channel?)
        //
        m_pPatternBody[n].num_pattern_lines = Swap2(m_pFileData->NextUI2());
        m_pPatternBody[n].pattern_lines = new OKTPatternLine_t[m_pPatternBody[n].num_pattern_lines];
        
        for (int i = 0; i < m_pPatternBody[n].num_pattern_lines; i++)
        {
            // pattern line
            OKTPatternLine_t *pLine = (OKTPatternLine_t*)m_pFileData->GetAtCurrent();
            
            m_pPatternBody[n].pattern_lines[i].newnote = pLine->newnote;
            m_pPatternBody[n].pattern_lines[i].instrument = pLine->instrument;
            m_pPatternBody[n].pattern_lines[i].effect = pLine->effect;
            m_pPatternBody[n].pattern_lines[i].data = pLine->data;
            
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + sizeof(OKTPatternLine_t));
        }
    }
    else if (chunkID == IFFTag("SBOD"))
    {
        // sample body 1..n (multiple in single module)
        
        // just byte* sample_data
    }

    // not finished yet..    
    return false;
}


/////////// public


COktalyzerPlayer::COktalyzerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pSampleData(nullptr)
    , m_pPatternBody(nullptr)
{
}

COktalyzerPlayer::~COktalyzerPlayer()
{
    delete [] m_pPatternBody;
    delete [] m_pSampleData;
}

/*
  About fileformat:
  there seems to kinds, memory dump model (no popularity)
  and IFF-chunk like structures.
  
  This implements the IFF-like format handling,
  no idea about the other one..

*/
bool COktalyzerPlayer::ParseFileInfo()
{
    auto pBuffer = m_pFileData->GetBegin();
    if (::memcmp(pBuffer, "OKTASONG", 8) != 0)
    {
        // unsupported format
        return false;
    }
    
    // past identifier (no file length here)
    m_pFileData->SetCurrentPos(8);

    // just parse IFF-chunks
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

