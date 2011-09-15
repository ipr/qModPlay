//////////////////////////////////////
//
// CAhxPlayer :
// attempt at new (mostly) portable player
// for AHX0 and AHX1
//
// Based on documentation by: Stuart Caie
//
// Note on implementation:
// could use bitfield-structs in some data
// but struct packing might be a problem with some compilers
// so I'd rather not do that and done in harder way..
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "AhxPlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"


// use buffer only, can switch implementation easily then..
CAhxPlayer::CAhxPlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_enAhxFormat(Unknown)
    , m_subsongList(nullptr)
    , m_positionList(nullptr)
    , m_trackListData(nullptr)
    , m_trackZero(nullptr)
    , m_sampleData(nullptr)
    , m_sampleNames(nullptr)
{
    // needed anyway?
    m_trackZero = new uint8_t[192];
    ::memset(m_trackZero, 0, 192);
}

CAhxPlayer::~CAhxPlayer()
{
    delete [] m_sampleNames;
    delete [] m_sampleData;
    delete m_trackZero;
    delete [] m_trackListData;
    delete [] m_positionList;
    delete [] m_subsongList;
}

bool CAhxPlayer::ParseFileInfo()
{
    m_pFileData->SetCurrentPos(0);
    uint8_t *pBuf = m_pFileData->GetNext(4);
    if (pBuf[0] == 'T' && pBuf[1] == 'H' && pBuf[2] == 'X')
    {
        // check fourth byte also (AHX0, AHX1)
        if (pBuf[3] == 0x00)
        {
            m_enAhxFormat = AHX0;
        }
        else if (pBuf[3] == 0x01)
        {
            m_enAhxFormat = AHX1;
        }
    }
    
    if (m_enAhxFormat == Unknown)
    {
        // unsupported format
        return false;
    }
    
    // actually, don't use this as offset..
    uint16_t byteskip = ReadBEUI16();
    
    // some flags
    uint8_t temp = ReadUI8();
    
    // if track zero is saved in file or not
    // note: must be 1/0 for later..
    m_trackZeroSaved = ((temp & 0xF0) ? 1 : 0);
    m_playSpeed = (temp & 0x0F);
    
    m_posListLen = (ReadBEUI16() & 0xFF); // LEN
    m_restartPoint = ReadBEUI16();
    
    m_trackLen = ReadUI8(); // TRL
    m_trackCount = ReadUI8(); // TRK
    m_sampleCount = ReadUI8();
    m_subsongCount = ReadUI8();
    
    // subsong list
    if (m_subsongCount > 0)
    {
        m_subsongList = new AHXSubsongEntry_t[m_subsongCount];
        for (int i = 0; i < m_subsongCount; i++)
        {
            // just a value, nothing more to it?
            m_subsongList[i].m_song = ReadBEUI16();
        }
    }
    
    // position list
    if (m_posListLen > 0)
    {
        m_positionList = new AHXPositionEntry_t[m_posListLen];
        for (int i = 0; i < m_posListLen; i++)
        {
            // 8 bytes, 4 sets (for each audiochannel), pair of bytes in each set
            m_positionList[i].setFromArray(m_pFileData->GetNext(8));
        }
    }
    
    // track list (zero count is valid also)
    if (m_trackCount > 0)
    {
        m_trackListSize = m_trackCount*m_trackLen;
        m_trackListSize += m_trackZeroSaved; // if saved or not.. this will cause problems..
        m_trackListData = new AHXTrackEntry_t[m_trackListSize];
        for (int i = 0; i < m_trackListSize; i++)
        {
            m_trackListData[i].setFromArray(m_pFileData->GetNext(3));
        }
    }
    
    // samples
    if (m_sampleCount > 0)
    {
        m_sampleData = new AHXSampleEntry_t[m_sampleCount];
        for (int i = 0; i < m_sampleCount; i++)
        {
            m_sampleData[i].setFromArray(m_pFileData->GetNext(22));
            
            // get length of playlist
            uint8_t playlistLen = m_sampleData[i].getPlaylistLen();
            
            // playlist data..
            m_sampleData[i].m_playlist = new AHXPlaylistEntry_t[playlistLen];
            for (int j = 0; j < playlistLen; j++)
            {
                m_sampleData[i].m_playlist[j].setFromArray(m_pFileData->GetNext(4));
            }
        }
    }
    
    // names, filename should be first
    size_t nPos = m_pFileData->GetCurrentPos();
    size_t nLen = m_pFileData->GetSize();
    if (nPos < nLen)
    {
        m_fileName = (char*)m_pFileData->GetAtCurrent(); // should be NULL-terminated
        m_pFileData->SetCurrentPos(nPos + m_fileName.length());
    }

    // sample names
    if (m_sampleCount > 0)
    {
        m_sampleNames = new std::string[m_sampleCount];
        int i = 0;
        while ((nLen - nPos) > 1)
        {
            nPos = m_pFileData->GetCurrentPos();
            
            m_sampleNames[i] = (char*)m_pFileData->GetAtCurrent();
            nPos += m_sampleNames[i].length();
            
            m_pFileData->SetCurrentPos(nPos);
            i++;
        }
    }
    return true;
}

DecodeCtx *CAhxPlayer::PrepareDecoder()
{
    // use default implementation for now..
    m_pDecodeCtx = new DecodeCtx();
    
    
    return m_pDecodeCtx;
}

size_t CAhxPlayer::DecodePlay(void *pBuffer, const size_t nBufSize)
{
    // 

    return 0;    
}
