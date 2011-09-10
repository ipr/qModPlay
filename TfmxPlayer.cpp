//////////////////////////////////////
//
// CTfmxPlayer : 
// handle TFMX modules by Chris Huelsbeck
//
// Some (rough) text files exist for format,
// some C-code for Amiga library and some for a Unix/Linux app..
// Should be possible to create new player for it..
//
// So far based on documentation by: Jonathan H. Pickard
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "TfmxPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


CTfmxPlayer::CTfmxPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pSongStartPositions(nullptr)
    , m_pSongEndPositions(nullptr)
    , m_pTempoNumbers(nullptr)
{
}

CTfmxPlayer::~CTfmxPlayer()
{
    delete m_pSongStartPositions;
    delete m_pSongEndPositions;
    delete m_pTempoNumbers;
}

/*
 some c-code for Amiga exists at least..
 
 note: cyb tfmx module has mdat and smpl in one file?
 -> variations of separate/concantenated files exist?

*/

bool CTfmxPlayer::ParseFileInfo()
{
    auto pBuffer = m_pFileData->GetBegin();
    if (::memcmp(pBuffer, "TFMX-SONG ", 10) != 0)
    {
        // unsupported format
        return false;
    }
    
    // skip identifier 
    // + unused word
    // + unused long
    m_pFileData->SetCurrentPos(16);
    
    // 40x6 text area
    m_textArea.assign(m_pFileData->GetAtCurrent(), 240);
    m_pFileData->SetCurrentPos(m_pFileData->GetAtCurrent() + 240);
    
    // array of 96 words:
    // - 1st 32 are song start positions
    // - 2nd 32 are song end positions
    // - 3rd 32 are tempo numbers
    // -- if greater than 15 -> beats-per-minute, 
    //    a beat taking 24 jiffies
    // -- otherwise divide-by-value into frequency of 50Hz
    //    (0=50Hz, 1=25Hz, 2=16.7 Hz...)
    m_pSongStartPositions = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongStartPositions[i] = Swap2(m_pFileData->NextUI2());
    }
    m_pSongEndPositions = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongStartPositions[i] = Swap2(m_pFileData->NextUI2());
    }
    m_pTempoNumbers = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongStartPositions[i] = Swap2(m_pFileData->NextUI2());
    }
    
    // Packed modules:
    // offset $1D0: three longs, offsets into the file
    // - trackstep
    // - pattern data pointers (near end of file)
    // - macro data pointers (near end of file)
    //
    // Unpacked modules:
    // three longwords at $1D0 are null
    // Fixed offsets of $600,$200,$400 apply.
    
    return true;    
}

