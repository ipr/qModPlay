//////////////////////////////////////
//
// COktalyzerPlayer :
//
// just interface for now
//
// Based on documentation by: Harald Zappe
// (Date: Wed, 6 Apr 94 19:48:56 +0200)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "OktalyzerPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


//////////// protected methods


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
        
        size_t nSampleCount = (chunkLen / sizeof(OKTSampleDirectory_t));
        for (int i = 0; i < nSampleCount; i++)
        {
            OKTSampleDirectory_t *pSampleDir = (OKTSampleDirectory_t*)m_pFileData->GetAtCurrent();
            
            // ..byteswap & store..
            
            m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + sizeof(OKTSampleDirectory_t));
        }
        //return true;
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
        // song length
        m_SongLength = Swap2(m_pFileData->NextUI2());
        return true;
    }
    else if (chunkID == IFFTag("PLEN"))
    {
        // number of pattern positions
        m_NumPositions = Swap2(m_pFileData->NextUI2());
        return true;
    }
    else if (chunkID == IFFTag("PATT"))
    {
        // pattern positions
        // seems to be fixed length chunk?
        // note: zero *IS* valid for this..
        m_PatternPositions.m_nLen = chunkLen;
        m_PatternPositions.m_pBuf = new uint8_t[m_PatternPositions.m_nLen];
        ::memcpy(m_PatternPositions.m_pBuf, m_pFileData->GetAtCurrent(), m_PatternPositions.m_nLen);
        
        return true;
    }
    else if (chunkID == IFFTag("PBOD"))
    {
        // pattern body 1
    }
    else if (chunkID == IFFTag("SBOD"))
    {
        // sample body 1 
    }

    // not finished yet..    
    return false;
}


/////////// public


COktalyzerPlayer::COktalyzerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

COktalyzerPlayer::~COktalyzerPlayer()
{
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

