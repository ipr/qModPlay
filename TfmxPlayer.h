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

#ifndef TFMXPLAYER_H
#define TFMXPLAYER_H

#include "ModPlayer.h"

#include <string>

class CTfmxPlayer : public CModPlayer
{
protected:
    
    // 40x6 text area
    std::string m_textArea;
    
    // arrays of words, 32 words each
    uint16_t *m_pSongStartPositions;
    uint16_t *m_pSongEndPositions;
    uint16_t *m_pTempoNumbers;
    
    bool m_bIsPacked;

    // offsets to file    
    uint32_t m_trackStepPtr;
    uint32_t m_patternDataPtr;
    uint32_t m_macroDataPtr;

    // 
    uint16_t m_trackStepData[8];

    //
    bufferedData_t m_patternData;
    

    
public:
    CTfmxPlayer(CReadBuffer *pFileData);
    virtual ~CTfmxPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // TFMXPLAYER_H
