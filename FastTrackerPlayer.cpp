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


//////////// protected methods


// TODO: check needed parameters and where to call..
void CFastTrackerPlayer::OnPatternData()
{
    // TODO: can we just loop through each pattern?
    // (check storage)
    for (int i = 0; i < m_channelCount; i++)
    {
        // we should know channel count by now..
        m_patterns[i].m_packedSize = (m_channelCount*64);
        
        // TODO: check if this is empty and skip data if so (not stored)
        
        m_patterns[i].m_patternHeaderLen = ReadLEUI32();
        m_patterns[i].m_packingType = ReadUI8();
        m_patterns[i].m_rowCount = ReadLEUI16();
        m_patterns[i].m_packedSize = ReadLEUI16();
        
        // ..actual pattern data follows?
    }
}


/////////// public


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
    
    // TODO: this is wrong? (should be by patterncount instead of channelcount..?)
    // allocate area for pattern data, including empty patterns (which aren't saved):
    // size amount of channels, each pattern at least 64 bytes
    m_patterns = new FTPattern[m_channelCount];
    
    
    return true;
}
