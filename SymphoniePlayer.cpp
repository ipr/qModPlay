#include "SymphoniePlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CSymphoniePlayer::CSymphoniePlayer(const CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

bool CSymphoniePlayer::ParseFileInfo()
{
    return false;
    
}


