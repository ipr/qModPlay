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

#ifndef DIGIBOOSTERPLAYER_H
#define DIGIBOOSTERPLAYER_H

#include "ModPlayer.h"


class CDigiBoosterPlayer : public CModPlayer
{
protected:

    // 128 bytes of data
    uint8_t *m_pOrders;    
    
    char m_versionName[4]; // version as string
    uint8_t m_versionNumber; // version as byte
    
public:
    CDigiBoosterPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPLAYER_H
