//////////////////////////////////////
//
// CScreamTrackerPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef SCREAMTRACKERPLAYER_H
#define SCREAMTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>


class CScreamTrackerPlayer : public CModPlayer
{
public:
    CScreamTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CScreamTrackerPlayer();
};

#endif // SCREAMTRACKERPLAYER_H
