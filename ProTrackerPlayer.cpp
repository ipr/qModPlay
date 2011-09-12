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


#include "ProTrackerPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CProTrackerPlayer::CProTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

CProTrackerPlayer::~CProTrackerPlayer()
{
}

bool CProTrackerPlayer::ParseFileInfo()
{
    
    // pretty normal IFF-chunk format
    
    return true;
}

