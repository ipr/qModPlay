//////////////////////////////////////
//
// CDigiBoosterPlayer :
// rewrite of DigiBooster and DigiBooster PRO player 
// in (mostly) portable C++
//
// Based on M68k assembler and E-language source codes..
// (well, at least there _are_ sources..)
//
// Just interface for now..
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef DIGIBOOSTERPLAYER_H
#define DIGIBOOSTERPLAYER_H

#include "ModPlayer.h"


class CDigiBoosterPlayer : public CModPlayer
{
public:
    CDigiBoosterPlayer(const CReadBuffer *pFileData);
    virtual ~CDigiBoosterPlayer() {}
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPLAYER_H
