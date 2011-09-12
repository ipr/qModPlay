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


#include "ImpulseTrackerPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


CImpulseTrackerPlayer::CImpulseTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

CImpulseTrackerPlayer::~CImpulseTrackerPlayer()
{
}

