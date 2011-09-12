//////////////////////////////////////
//
// CProTrackerPlayer :
//
// just interface for now
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef PROTRACKERPLAYER_H
#define PROTRACKERPLAYER_H

#include "ModPlayer.h"

#include <string>

class CProTrackerPlayer : public CModPlayer
{
public:
    CProTrackerPlayer(CReadBuffer *pFileData);
    virtual ~CProTrackerPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // PROTRACKERPLAYER_H
