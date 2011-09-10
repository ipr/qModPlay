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


#include "AhxPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


CAhxPlayer::CAhxPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

CAhxPlayer::~CAhxPlayer()
{
}

bool CAhxPlayer::ParseFileInfo()
{
    auto pBuffer = m_pFileData->GetBegin();
    if (::memcmp(pBuffer, "THX", 3) != 0)
    {
        // unsupported format
        return false;
    }
    
    return true;    
}

