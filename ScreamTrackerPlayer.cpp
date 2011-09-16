//////////////////////////////////////
//
// CScreamTrackerPlayer :
//
// Based on documentation by unknown..
// (sorry, no name on the file..)
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "ScreamTrackerPlayer.h"


//////////// protected methods

// TODO: implement
void CScreamTrackerPlayer::ParseSampleData()
{
}


/////////// public

CScreamTrackerPlayer::CScreamTrackerPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pOrders(nullptr)
    , m_pInstruments(nullptr)
    , m_pPatterns(nullptr)
{
}

CScreamTrackerPlayer::~CScreamTrackerPlayer()
{
    delete m_pPatterns;
    delete m_pInstruments;
    delete m_pOrders;
}

bool CScreamTrackerPlayer::ParseFileInfo()
{
    if (::memcmp(m_pFileData->GetAt(44), "SCRM", 4) != 0)
    {
        // unsupported format/unknown identifier
        return false;
    }
    
    // song name, fixed length with NULL
    m_songName.assign(m_pFileData->GetBegin(), 28);
    m_pFileData->SetCurrentPos(28);

    // actually, skip some four bytes (check later..)
    // (1Ah, Typ, x, x)
    m_pFileData->SetCurrentPos(0x20);
    
    m_orderCount = ReadLEUI16(); 
    m_instrumentCount = ReadLEUI16(); 
    m_patternCount = ReadLEUI16(); 

    m_flags = ReadLEUI16();
    m_Cwtv = ReadLEUI16();
    m_Ffv = ReadLEUI16();
    
    if (m_Ffv != OLD_SIGNED_SAMPLES 
        && m_Ffv != STANDARD_UNSIGNED_SAMPLES)
    {
        // file format version unsupported here,
        // currently anyway..
        return false;
    }
    
    // skip format identifier (checked at beginning)
    m_pFileData->SetCurrentPos(0x30);
    
    m_globalVolume = ReadUI8();
    m_initialSpeed = ReadUI8();
    m_initialTempo = ReadUI8();
    m_masterVolume = ReadUI8();
    
    // skip some..
    m_pFileData->SetCurrentPos(0x3E);
    
    m_special = ReadLEUI16(); // custom data offset
    
    // 32 bytes: channel settings for 32 channels
    m_pFileData->NextArray(m_channelSettings, 32);

    size_t nSize = m_orderCount*sizeof(uint8_t);
    if (nSize % 2 != 0)
    {
        // padding needed? "should be even" ?
        nSize += 1;
    }
    m_pOrders = new uint8_t[nSize];
    m_pFileData->NextArray(m_pOrders, nSize);

    // actually, "parapointers" which are just weird offsets to file..
    m_pInstruments = new uint16_t[m_instrumentCount];
    m_pFileData->NextArray(m_pInstruments, m_instrumentCount*sizeof(uint16_t));
    if (m_bBigEndian)
    {
        for (int i = 0; i < m_instrumentCount; i++)
        {
            m_pInstruments[i] = Swap2(m_pInstruments[i]);
        }
    }
    
    // actually, "parapointers" which are just weird offsets to file..
    m_pPatterns = new uint16_t[m_patternCount];
    m_pFileData->NextArray(m_pPatterns, m_patternCount*sizeof(uint16_t));
    if (m_bBigEndian)
    {
        for (int i = 0; i < m_patternCount; i++)
        {
            m_pPatterns[i] = Swap2(m_pPatterns[i]);
        }
    }
    
    // TODO: sample-handling etc.
    
    
    return true;
}

DecodeCtx *CScreamTrackerPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
    
    return m_pDecodeCtx;
}
