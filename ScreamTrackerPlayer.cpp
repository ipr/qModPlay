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


#include "ScreamTrackerPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


CScreamTrackerPlayer::CScreamTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

CScreamTrackerPlayer::~CScreamTrackerPlayer()
{
}

