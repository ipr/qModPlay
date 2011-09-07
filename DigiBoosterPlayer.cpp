#include "DigiBoosterPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CDigiBoosterPlayer::CDigiBoosterPlayer(const CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
}

bool CDigiBoosterPlayer::ParseFileInfo()
{
    auto pPos = m_pFileData->GetBegin();
    if (::memcmp(pPos, "DIGI Booster module\0", 20) != 0)
    {
        // unsupported format
        return false;
    }
    
    // skip a bit..
    
    // quick hack..
    uint8_t nChannels = m_pFileData->GetAt(25)[0];
    uint8_t nPatterns = m_pFileData->GetAt(46)[0];
    uint8_t nOrders = m_pFileData->GetAt(47)[0];

    /*
    while (pPos < m_pFileData->GetEnd())
    {
        
        
    }
    */

    return true;    
}

