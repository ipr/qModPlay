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
    , m_pSampleInfo(nullptr)
    , m_songPositions()
{
}

CProTrackerPlayer::~CProTrackerPlayer()
{
    delete m_songPositions.m_pBuf;
    delete [] m_pSampleInfo;
}

// TODO: file type identification ??
// no proper header in format?
//
bool CProTrackerPlayer::ParseFileInfo()
{
    if (m_pFileData->GetSize() < 1084)
    {
        // not enough data for a ProTracker module
        // (fixed-size before this, identifier here)
        return false;
    }
    
    // note: might not exist in some cases
    // or variations may have something else..
    if (::memcmp(m_pFileData->GetAt(1080), "M.K.", 4) != 0)
    {
        // .. not ProTracker module?
        return false;
    }
    
    // starts with song name? (fixed-length)
    m_songname.assign((char*)m_pFileData->GetBegin(), 20);
    m_pFileData->SetCurrentPos(20);
    
    m_pSampleInfo = new PTSampleInfo[31];
    for (int i = 0; i < 31; i++)
    {
        m_pSampleInfo[i].m_name.assign((char*)m_pFileData->GetAtCurrent(), 22);
        m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() +22);
        
        m_pSampleInfo[i].m_length = Swap2(m_pFileData->NextUI2());
        m_pSampleInfo[i].m_finetune = (m_pFileData->NextUI1() & 0x0F);
        m_pSampleInfo[i].m_volume = m_pFileData->NextUI1();

        // note: keep offset in bytes for simplicity later
        m_pSampleInfo[i].m_repeatPoint = Swap2(m_pFileData->NextUI2()) * 2;
        m_pSampleInfo[i].m_repeatLength = Swap2(m_pFileData->NextUI2()) * 2;
    }
    
    m_songLength = m_pFileData->NextUI1();
    m_mysterybyte = m_pFileData->NextUI1();
    
    m_songPositions.m_nLen = 128;
    m_songPositions.m_pBuf = new uint8_t[m_songPositions.m_nLen];
    ::memcpy(m_songPositions.m_pBuf, m_pFileData->GetAtCurrent(), m_songPositions.m_nLen);
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + m_songPositions.m_nLen);
    
    
    // TODO: continue..
    
    return true;
}

