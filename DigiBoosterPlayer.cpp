//////////////////////////////////////
//
// CDigiBoosterPlayer :
// player for DigiBooster modules
// in (mostly) portable C++
//
// Based on M68k assembler and E-language source codes..
// (well, at least there _are_ sources..)
//
// Based on documentation by: Claudio Matsuoka
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "DigiBoosterPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

CDigiBoosterPlayer::CDigiBoosterPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pOrders(nullptr)
{
}

CDigiBoosterPlayer::~CDigiBoosterPlayer()
{
    delete m_pOrders;
}

bool CDigiBoosterPlayer::ParseFileInfo()
{
    auto pPos = m_pFileData->GetBegin();
    if (::memcmp(pPos, "DIGI Booster module\0", 20) != 0)
    {
        // unsupported format
        return false;
    }
    
    // version string
    ::memcpy(m_version, pPos + 20, 4);
    m_versionNumber = m_pFileData->GetAt(24)[0];
    
    uint8_t nChannels = m_pFileData->GetAt(25)[0];

    // unknown segment..
    
    uint8_t nPatterns = m_pFileData->GetAt(46)[0];
    uint8_t nOrders = m_pFileData->GetAt(47)[0];

    // should use value from above?
    //::memcpy(m_pOrders, m_pFileData->GetAt(48), nOrders);
    ::memcpy(m_pOrders, m_pFileData->GetAt(48), 128);
    
    /*
    while (pPos < m_pFileData->GetEnd())
    {
        
        
    }
    */

    return true;    
}

