//////////////////////////////////////
//
// CDigiBoosterPlayer :
// player for DigiBooster modules
// in (mostly) portable C++
//
// Based on M68k assembler and E-language source codes..
// (well, at least there _are_ sources..)
//
// Based on documentation by: Claudio Matsuoka
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef DIGIBOOSTERPLAYER_H
#define DIGIBOOSTERPLAYER_H

#include "ModPlayer.h"


class CDigiBoosterPlayer : public CModPlayer
{
public:
    CDigiBoosterPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPLAYER_H
