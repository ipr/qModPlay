//////////////////////////////////////
//
// CDigiBoosterPlayer :
// player for DigiBooster modules
// in (mostly) portable C++
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

#include <string>

class CDigiBoosterPlayer : public CModPlayer
{
protected:
    
    // actual song data?
    bufferedData_t m_SongData;
    
    // always fixed-size array of fixed-length strings?
    std::string *m_pSampleNames;
    
    std::string m_songName;
    
    ///////
    // always 31 values in these
    uint32_t *m_pSampleLength;
    uint32_t *m_pSampleLoopStart;
    uint32_t *m_pSampleLoopLength;
    uint8_t *m_pSampleVolumes;
    uint8_t *m_pSampleFinetunes;
    ///////
    
    // 128 bytes of data,
    // is it always? or m_orderCount?
    bufferedData_t m_Orders;
    
    uint8_t m_channelCount;
    uint8_t m_patternsCount;
    uint8_t m_orderCount;
    
    // size??? assuming byte
    uint8_t m_packEnable;
    
    char m_versionName[4]; // version as string
    uint8_t m_versionNumber; // version as byte
    
public:
    CDigiBoosterPlayer(CReadBuffer *pFileData);
    virtual ~CDigiBoosterPlayer();
    
    virtual bool ParseFileInfo();
};

#endif // DIGIBOOSTERPLAYER_H
