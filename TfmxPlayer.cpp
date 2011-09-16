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


//////////// protected methods


// process command/note from pattern data
void CTfmxPlayer::OnPatternCommand(uint32_t command, uint8_t *pOutBuf, const size_t nLen)
{
    // decode command and parameters
    // note: following now expects little-endian byteorder,
    // fix it later ;)
    
    switch (command & 0xFF000000)
    {
    case 0xF0:
        // end pattern, advance trackstep
        break;
        
    case 0xF1:
        // loop repeat block
        {
            uint8_t count = (command & 0x00FF0000);
            uint16_t blockStart = (command & 0x0000FFFF);
            //CommandRepeatBlock(count, blockStart);
        }
        break;
        
    case 0xF2:
        // jump into pattern
        {
            uint8_t pattern = (command & 0x00FF0000);
            uint16_t point = (command & 0x0000FFFF);
            //CommandJumpIntoPattern(pattern, point);
        }
        break;
        
    case 0xF3:
        {
            uint8_t jiffies = (command & 0x00FF0000);
            // insert empty on output ?
            //WaitFor(jiffies+1);
        }
        break;
        
    case 0xF4:
        // stop, disable track
        break;
        
    case 0xF5:
        // set flag in voice
        //uint8_t v = ..
        break;
        
    case 0xF6:
        // vibrato
        break;
        
    case 0xF7:
        // envelope
        break;
        
    case 0xF8:
        // gosub
        break;
        
    case 0xF9:
        // restore program counter and continue execution
        break;
        
    case 0xFA:
        // fade master volume
        break;
        
    case 0xFB:
        // play pattern
        break;
        
    case 0xFC:
        // portamento
        break;
        
    case 0xFD:
        // lock channel for n ticks
        {
            uint8_t channel = (command & 0x00FF0000);
            uint16_t ticks = (command & 0x0000FFFF);
            //Command..(channel, ticks);
        }
        break;
        
    case 0xFE:
        // stop custom pattern (see 0xF4)
        break;
        
    case 0xFF:
        // noop
        break;
        
    default:
        // silence GCC..
        break;
    }
    
}

// process macro command
void CTfmxPlayer::OnMacroCommand(uint32_t command, uint8_t *pOutBuf, const size_t nLen)
{
    // decode command and parameters
    // note: following now expects little-endian byteorder,
    // fix it later ;)
    
    switch (command & 0xFF000000)
    {
    case 0x00:
        // DMAoff : stop all effects, kill voice
        break;
        
    case 0x01:
        // DMAon : start voice
        break;
        
    case 0x02:
        // set begin
        {
            uint32_t value = (command & 0x00FFFFFF);
        }
        break;
        
        // ...
        
    default:
        // silence GCC..
        break;
    }
    
}


/////////// public


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
        m_pSongStartPositions[i] = ReadBEUI16();
    }
    
    // song end positions
    m_pSongEndPositions = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pSongEndPositions[i] = ReadBEUI16();
    }
    
    // tempo numbers
    // - if greater than 15 -> beats-per-minute, 
    //    a beat taking 24 jiffies
    // - otherwise divide-by-value into frequency of 50Hz
    //    (0=50Hz, 1=25Hz, 2=16.7 Hz...)
    m_pTempoNumbers = new uint16_t[32];
    for (int i = 0; i < 32; i++)
    {
        m_pTempoNumbers[i] = ReadBEUI16();
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
    m_trackStepPtr = ReadBEUI32();
    // $1D4
    m_patternDataPtr = ReadBEUI32();
    // $1D8
    m_macroDataPtr = ReadBEUI32();
    
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
        m_trackStepData[i] = ReadBEUI16();
    }

    // get pattern data:
    // 32-bit offsets to MDAT-file,
    // max. 128 patterns per song-file
    m_pFileData->SetCurrentPos(m_patternDataPtr);
    //
    // each pattern is 32-bit command:
    // upper-most byte as command id, three bytes as parameter..

    // get macro data (similar to pattern data):
    // 32-bit offsets to 32-bit command-values
    m_pFileData->SetCurrentPos(m_macroDataPtr);
    
    return true;
}

