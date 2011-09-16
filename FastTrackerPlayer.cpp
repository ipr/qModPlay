//////////////////////////////////////
//
// CFastTrackerPlayer :
//
// just interface for now..
//
// Based on documentation By Mr.H of Triton in 1994
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "FastTrackerPlayer.h"

CFastTrackerPlayer::CFastTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_patternOrders()
{
}

CFastTrackerPlayer::~CFastTrackerPlayer()
{
    delete m_patternOrders.m_pBuf;
}

bool CFastTrackerPlayer::ParseFileInfo()
{
    if (::memcmp(m_pFileData->GetNext(17), "Extended module: ", 17) != 0)
    {
        // unsupported format
        return false;
    }
    
    // mod name
    m_moduleName.assign((char*)m_pFileData->GetNext(20), 20);
    m_mystery = ReadUI8(); // $1a
    m_trackerName.assign((char*)m_pFileData->GetNext(20), 20);
    m_version = ReadLEUI16();
    
    // no need to keep, just for debugging now..
    m_headerSize = ReadLEUI32(); 
    
    m_songLength = ReadLEUI16();
    m_restartPosition = ReadLEUI16();
    m_channelCount = ReadLEUI16();
    m_patternCount = ReadLEUI16();
    m_instrumentCount = ReadLEUI16();
    m_flags = ReadLEUI16();
    m_defaultTempo = ReadLEUI16();
    m_defaultBPM = ReadLEUI16();
    
    m_patternOrders.m_nLen = 256;
    m_patternOrders.m_pBuf = new uint8_t[m_patternOrders.m_nLen];
    m_pFileData->NextArray(m_patternOrders.m_pBuf, m_patternOrders.m_nLen);
    
    
    return true;
}
