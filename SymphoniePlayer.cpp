//////////////////////////////////////
//
// CSymphoniePlayer :
// player for Symphonie modules (SymMOD)
// in (mostly) portable C++
//
// Based on M68k assembler and Java source codes 
// by: Patrick Meng 2008
// 
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#include "SymphoniePlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"

#include <string>


//////////// protected methods


// unpack simple "runlength" packing of chunk data
bool CSymphoniePlayer::UnpackRunlen(const uint8_t *pOrigData, const size_t nLen, uint8_t *pOutBuf, size_t nUnpackLen)
{
    size_t nPos = 0;
    size_t nOutPos = 0;
    while (nPos < nLen)
    {
        uint8_t packMode = pOrigData[nPos];
        nPos++;
        switch (packMode)
        {
        case 0:
            {
                // copy as-is next bytes (upto 255)
                uint8_t byteCount = pOrigData[nPos];
                nPos++;
                ::memcpy(pOutBuf + nOutPos, pOrigData + nPos, byteCount);
                nOutPos += byteCount;
            }
            break;
            
        case 3:
            {
                // upto 255 bytes of zero
                uint8_t byteCount = pOrigData[nPos];
                nPos++;
                ::memset(pOutBuf + nOutPos, 0, byteCount);
                nOutPos += byteCount;
            }
            break;
        
        case 2:
            {
                // 2x 32-bit long value
                uint32_t *pVal = (uint32_t*)(pOrigData + nPos);
                nPos += 4;
                uint32_t val = Swap4(*pVal);
                ::memcpy(pOutBuf + nOutPos, &val, 4);
                nOutPos += 4;
                ::memcpy(pOutBuf + nOutPos, &val, 4);
                nOutPos += 4;
            }
            break;
            
        case 1:
            {
                // copy next n 32-bit longs to out
                uint8_t byteCount = pOrigData[nPos];
                nPos++;
                uint32_t *pVal = (uint32_t*)(pOrigData + nPos);
                nPos += 4;
                uint32_t val = Swap4(*pVal);
                for (int i = 0; i < byteCount; i++)
                {
                    ::memcpy(pOutBuf + nOutPos, &val, 4);
                    nOutPos += 4;
                }
            }
            break;
            
        case 0xFF:
            // noop
            break;
            
        default:
            // unsupported packing mode (bug on?)
            return false;
        }
    }
    
    return true;
}

// TODO: implement
// unpack delta compression of sample data
bool CSymphoniePlayer::Decode8bitSample(const uint8_t *pData, const size_t nLen)
{
    return false;
}

// TODO: implement
// unpack delta compression of sample data
bool CSymphoniePlayer::Decode16bitSample(const uint8_t *pData, const size_t nLen)
{
    return false;
}


// some chunks have just single parameter in 4 bytes
// and no length of chunk; 
// some have varying data and need more handling (length given..)
bool CSymphoniePlayer::OnChunk(uint32_t chunkID)
{
    // debug..
    long lId = chunkID;
    bool bHandled = true;
    size_t nPos = m_pFileData->GetCurrentPos();


    switch (chunkID)
    {
    case CT_CHANNELNUMB:
        m_audioChannelcount = Swap4(m_pFileData->NextUI4());
        break;
    case CT_TRACKLEN:
        m_trackLength = Swap4(m_pFileData->NextUI4());
        break;
    case CT_PATTERNNUMB:
        m_totalPatternlength = Swap4(m_pFileData->NextUI4());
        break;
    case CT_SAMPLENUMB:
        // 16-bit value in 4 bytes? -> get word only
        m_instrumentCount = (Swap4(m_pFileData->NextUI4()) & 0x0000FFFF);
        break;
    case CT_NOTESIZE:
        // 16-bit value in 4 bytes? -> get word only
        m_songEventSize = (Swap4(m_pFileData->NextUI4()) & 0x0000FFFF);
        break;
    case CT_SYSTEMSPEED:
        m_systemBPM = Swap4(m_pFileData->NextUI4());
        break;
    case CT_ISSONG:
        m_isPureSong = Swap4(m_pFileData->NextUI4());
        break;
        
    case CT_NGSAMPLEBOOST:
        m_globalSampleBoost = Swap4(m_pFileData->NextUI4());
        break;
    case CT_PITCHDIFF:
        m_stereoEncPitchDiff = Swap4(m_pFileData->NextUI4());
        break;
    case CT_SAMPLEDIFF:
        m_stereoEncSampleDiff = Swap4(m_pFileData->NextUI4());
        break;
        
    case CT_SONGDATA:
    case CT_SAMPLE:
    case CT_NOTEDATA:
    case CT_SAMPLENAMES:
    case CT_SEQUENCE:
    case CT_INFOTEXT:
    case CT_DELTASAMPLE:
    case CT_DELTA16:
    case CT_INFOTYPE:
    case CT_INFOOBJ:
    case CT_STRING:
        bHandled = OnLargeChunk(chunkID);
        break;
        
    default:
        // we should skip unknown chunks
        // but we don't know size of it..
        return false;
    }
    
    // not supported chunk yet..
    return bHandled;
}


bool CSymphoniePlayer::OnLargeChunk(uint32_t chunkID)
{
    bool bHandled = true;
    size_t nPos = m_pFileData->GetCurrentPos();
    uint32_t chunkLen = Swap4(m_pFileData->NextUI4());
    
    uint8_t *pData = m_pFileData->GetAtCurrent();
    if (::memcmp(pData, "PACK", 4) == 0
        && pData[4] == 0xFF && pData[5] == 0xFF
        && chunkLen > 16)
    {
        // runlen packing of chunk data -> unpack to buffer

        // update offset
        m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 6);
       
        uint32_t unpackedLen = Swap4(m_pFileData->NextUI4());
        pData = new uint8_t[unpackedLen];
        
        if (UnpackRunlen(m_pFileData->GetAtCurrent(), chunkLen, pData, unpackedLen) == false)
        {
            // bug or corrupted file detected
            return false;
        }
        
        // update offset
        m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + chunkLen);
        chunkLen = unpackedLen;
    }
    
    // process data, byteswap and such..
    switch (chunkID)
    {
    case CT_SONGDATA: // -10
        bHandled = OnSongDataPositions(pData, chunkLen);
        break;
    case CT_SAMPLE: // -11
        // instrument sample
        bHandled = OnInstrumentSample(pData, chunkLen);
        break;
    case CT_NOTEDATA: // -13
        // pattern data
        bHandled = OnPatternData(pData, chunkLen);
        break;
    case CT_SAMPLENAMES: // -14
        bHandled = OnSampleNames(pData, chunkLen);
        break;
    case CT_SEQUENCE: // -15
        bHandled = OnSequence(pData, chunkLen);
        break;
        
    case CT_INFOTEXT: // -16
        // just comments/text information, may be packed also?
        m_comments.assign((char*)m_pFileData->GetAtCurrent(), chunkLen);
        break;
        
    case CT_DELTASAMPLE: // -17
        // 8-bit deltapacked sample
        //bHandled = Decode8bitSample(pData, chunkLen);
        //bHandled = OnSampleData(pData, chunkLen);
        break;
    case CT_DELTA16: // -18
        // 16-bit deltapacked sample
        //bHandled = Decode16bitSample(pData, chunkLen);
        //bHandled = OnSampleData(pData, chunkLen);
        break;
        
    case CT_INFOTYPE: // -19
        break;
    case CT_INFOOBJ: // -20
        break;
    case CT_STRING: // -21
        break;
        
    default:
        return false;
    }

    // update position to next chunk    
    m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + chunkLen);
    return bHandled;
}

bool CSymphoniePlayer::OnPatternData(uint8_t *pData, const size_t nLen)
{
    // keep as is..
    m_PatternData.m_pBuf = pData;
    m_PatternData.m_nLen = nLen;
    
    size_t nPatternSize = m_audioChannelcount*m_trackLength*4;
    if (nPatternSize == 0 || m_PatternData.m_nLen == 0)
    {
        // no data
        return false;
    }
    
    m_nPatternCount = m_PatternData.m_nLen / nPatternSize;
    if (m_nPatternCount == 0)
    {
        return false;
    }
    
    
    return true;
}

// sample/instrument names
bool CSymphoniePlayer::OnSampleNames(uint8_t *pData, const size_t nLen)
{
    // ?? fixed size strings?
    int sampleNameCount = nLen / 256;
    uint8_t *pBuf = pData;
    
    for (int i = 0; i < sampleNameCount; i++)
    {
        // should be null-terminated string
        std::string sampleName = (char*)pBuf;
        
        if (sampleName == "ViRT")
        {
            // virtual instrument, only visual information?
            
            // TODO: keep somewhere..
        }
        else
        {
            // "normal" instrument
            
            // TODO: keep somewhere..
        }
        
        pBuf = (pBuf + 256);
    }
    
    return true;
}

bool CSymphoniePlayer::OnInstrumentSample(uint8_t *pData, const size_t nLen)
{
    return false;
}

bool CSymphoniePlayer::OnSequence(uint8_t *pData, const size_t nLen)
{
    return false;
}

bool CSymphoniePlayer::OnSongDataPositions(uint8_t *pData, const size_t nLen)
{
    return false;
}



/////////// public

CSymphoniePlayer::CSymphoniePlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pInstruments(nullptr)
    , m_PatternData()
    , m_nPatternCount(0)
    , m_comments()
    , m_audioChannelcount(0)
    , m_trackLength(0)
    , m_totalPatternlength(0)
    , m_instrumentCount(0)
    , m_songEventSize(0)
    , m_systemBPM(0)
    , m_isPureSong(0)
    , m_globalSampleBoost(0)
    , m_stereoEncPitchDiff(0)
    , m_stereoEncSampleDiff(0)
{
}

CSymphoniePlayer::~CSymphoniePlayer()
{
    delete m_PatternData.m_pBuf;
    delete [] m_pInstruments;
}


/* some file format notes..
  
    offset  |   type    |   desc
    ----------------------------
    0       |   UI4     |   ID
    4       |   I4      |   version, always 1?

    8       |   UI4     |   chunk ID
    ...

    .. not exactly IFF as chunks don't always have sizes given ..
*/

bool CSymphoniePlayer::ParseFileInfo()
{
    if (m_pFileData->NextUI4() != IFFTag("SymM"))
    {
        // not symphonie module
        return false;
    }
    
    // version, always 1?
    m_lVersion = Swap4(m_pFileData->NextUI4());
 
    while (m_pFileData->IsEnd() == false)
    {
		// keep our enumerations for clarity, swap ID..
        uint32_t chunkID = Swap4(m_pFileData->NextUI4());
        if (chunkID == CT_EOF)
        {
            // end of file?
            break;
        }
        else if (chunkID != CT_EMPTYSAMPLE)
        {
            if (OnChunk(chunkID) == false)
            {
                // not supported somewhere..
                return false;
            }
        }
    }
    
    return true;
}

// TODO: implement
// decode in parts to given playback buffer
// when playing module
size_t CSymphoniePlayer::DecodePlay(void *pBuffer, const size_t nBufSize)
{
    return 0;
}

