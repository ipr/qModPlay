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
        m_pSampleInfo = new OKTSampleDirectory_t[m_nSampleCount];
        for (int i = 0; i < m_nSampleCount; i++)
        {
            // byteswap & keep values
            
            OKTSampleDirectory_t *pSampleDir = (OKTSampleDirectory_t*)m_pFileData->GetAtCurrent();
            
            ::memcpy(m_pSampleInfo[i].Sample_Name, pSampleDir->Sample_Name, 20);
            m_pSampleInfo[i].Sample_Len = Swap4(pSampleDir->Sample_Len);
            m_pSampleInfo[i].Repeat_Start = Swap2(pSampleDir->Repeat_Start);
            m_pSampleInfo[i].Repeat_Len = Swap2(pSampleDir->Repeat_Len);
            m_pSampleInfo[i].Volume = pSampleDir->Volume;
            
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
        // first pattern body -> allocate all..
        if (m_pPatternBody == nullptr)
        {
            m_pPatternBody = new OKTPatternData[m_SongLength];
        }

        // TODO: index next body-chunk
        // or replace with nodelist..?
        size_t n; // current index ?
        
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
        return true;
    }
    else if (chunkID == IFFTag("SBOD"))
    {
        // sample body 1..n (multiple in single module)
        // use m_nSampleCount counted in "SAMP" ?
        //
        // just byte* sample_data
    }

    // not finished yet..    
    return false;
}

// effects given in sample-data..
void COktalyzerPlayer::OnEffect(OKTPatternLine_t &pattern, uint8_t *pOutbuf, const size_t nLen)
{
    // values in decimal
    // note: format uses 50Hz clock tick for effects
    // -> count in frame-time
    
    switch (pattern.effect)
    {
    case 1: // portamento down
        // decrease period of current sample by 'data' once every 50Hz clock tick
        // (see PrepareDecoder(), conversion to frame-time)
        break;
    case 2: // portamento up
        
        // increase period of current sample by 'data' once every 50Hz clock tick
        // (see PrepareDecoder(), conversion to frame-time)
        break;
        
    case 10: // Arpeggio 3
        break;
    case 11: // Arpeggio 4
        break;
    case 12: // Arpeggio 5
        break;
        
    case 13:
        // decrease note number by 'data' once per tick
        break;
    case 17:
        // increase note number by 'data' once per tick
        break;
        
    case 21:
        // decrease note number by 'data' once per line
        break;
    case 30:
        // increase note number by 'data' once per line
        break;
        
    case 15:
        // Amiga low-pass filter control: 'data' indicates new setting
        break;
        
    case 25:
        // position jump: jump to pattern in 'data' 
        break;
        
    case 27:
        // release: start playing release phase
        break;

    case 28:
        // set speed to 50Hz ticks in 'data'
        break;
        
    case 31:
        // volume control: 
        if (pattern.data <= 0x40)
        {
        }
        else if (pattern.data > 0x40 && pattern.data <= 0x50)
        {
        }
        else if (pattern.data > 0x50 && pattern.data <= 0x60)
        {
        }
        else if (pattern.data > 0x60 && pattern.data <= 0x70)
        {
        }
        else if (pattern.data > 0x70 && pattern.data <= 0x80)
        {
        }
        break;
    }
}


/////////// public


COktalyzerPlayer::COktalyzerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pSampleInfo(nullptr)
    , m_pPatternBody(nullptr)
    , m_pSampleBody(nullptr)
{
}

COktalyzerPlayer::~COktalyzerPlayer()
{
    delete [] m_pSampleBody;
    delete [] m_pPatternBody;
    delete [] m_pSampleInfo;
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

DecodeCtx *COktalyzerPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
    // uses 50Hz ticks for timing?
    // -> convert to frametime (check samplerate)
    
    return m_pDecodeCtx;
}

// TODO:
size_t COktalyzerPlayer::DecodePlay(void *pBuffer, const size_t nBufSize)
{
    return 0;
}

