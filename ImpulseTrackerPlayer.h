//////////////////////////////////////
//
// CImpulseTrackerPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef IMPULSETRACKERPLAYER_H
#define IMPULSETRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>


class CImpulseTrackerPlayer : public CModPlayer
{
public:
    CImpulseTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CImpulseTrackerPlayer();
};

#endif // IMPULSETRACKERPLAYER_H
