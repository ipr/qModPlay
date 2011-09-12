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


#ifndef OKTALYZERPLAYER_H
#define OKTALYZERPLAYER_H

#include "ModPlayer.h"

#include <string>

class COktalyzerPlayer : public CModPlayer
{
protected:
    
    // CMOD chunk
    uint16_t m_chan_flags[4];
    
    bool OnChunk(uint32_t chunkID, const uint32_t chunkLen);
    
public:
    COktalyzerPlayer(CReadBuffer *pFileData);
    virtual ~COktalyzerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // OKTALYZERPLAYER_H
