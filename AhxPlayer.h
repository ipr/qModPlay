//////////////////////////////////////
//
// CAhxPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef AHXPLAYER_H
#define AHXPLAYER_H

#include "ModPlayer.h"


class CAhxPlayer : public CModPlayer
{
public:
    CAhxPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // AHXPLAYER_H
