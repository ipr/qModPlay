//////////////////////////////////////
//
// CDigiBoosterPlayer :
// rewrite of DigiBooster PRO player 
// in (mostly) portable C++
//
// Just interface for now..
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef DIGIBOOSTERPROPLAYER_H
#define DIGIBOOSTERPROPLAYER_H

#include "ModPlayer.h"


class CDigiBoosterProPlayer : public CModPlayer
{
public:
    CDigiBoosterProPlayer(const CReadBuffer *pFileData);
};

#endif // DIGIBOOSTERPROPLAYER_H
