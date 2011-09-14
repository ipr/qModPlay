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
    m_textArea.assign((char*)m_pFileData->GetAtCurrent(), 240);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 240);
    
    // array of 96 words:
    // - 1st 32 are song start positions
    // - 2nd 32 are song end positions
    // - 3rd 32 are tempo numbers
    
    // song start positions
    m_pSongStartPositions = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongStartPositions[i] = Swap2(m_pFileData->NextUI2());
    }
    
    // song end positions
    m_pSongEndPositions = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongEndPositions[i] = Swap2(m_pFileData->NextUI2());
    }
    
    // tempo numbers
    // - if greater than 15 -> beats-per-minute, 
    //    a beat taking 24 jiffies
    // - otherwise divide-by-value into frequency of 50Hz
    //    (0=50Hz, 1=25Hz, 2=16.7 Hz...)
    m_pTempoNumbers = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pTempoNumbers[i] = Swap2(m_pFileData->NextUI2());
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
    
    // $1D0
    m_trackStepPtr = Swap4(m_pFileData->NextUI4());
    // $1D4
    m_patternDataPtr = Swap4(m_pFileData->NextUI4());
    // $1D8
    m_macroDataPtr = Swap4(m_pFileData->NextUI4());
    
    // keep position..
    size_t nPos = m_pFileData->GetCurrentPos();
    
    m_bIsPacked = true; // assume packed
    if (m_trackStepPtr == 0 && m_patternDataPtr == 0 && m_macroDataPtr == 0)
    {
        // null -> not packed
        m_bIsPacked = false;
    }
    if (m_trackStepPtr == 0)
    {
        m_trackStepPtr = 0x600;
    }
    if (m_patternDataPtr == 0)
    {
        m_patternDataPtr = 0x200;
    }
    if (m_macroDataPtr == 0)
    {
        m_macroDataPtr = 0x400;
    }
    
    // get trackstep data
    m_pFileData->SetCurrentPos(m_trackStepPtr);
    for (int i = 0; i < 8; i++)
    {
        m_trackStepData[i] = Swap2(m_pFileData->NextUI2());
    }

    // get pattern data:
    // 32-bit offsets to MDAT-file
    m_pFileData->SetCurrentPos(m_patternDataPtr);

    // get macro data
    m_pFileData->SetCurrentPos(m_macroDataPtr);
    
    return true;
}

