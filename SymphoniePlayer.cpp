#include "SymphoniePlayer.h"

// for buffer-wrapper..
#include "AnsiFile.h"




//////////// protected methods


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
    case CT_EMPTYSAMPLE:
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
        bHandled = false;
        break;
        
    }

    // debug
    nPos = m_pFileData->GetCurrentPos();
    
    // not supported chunk yet..
    return bHandled;
}


bool CSymphoniePlayer::OnLargeChunk(uint32_t chunkID)
{
    bool bHandled = true;
    size_t nPos = m_pFileData->GetCurrentPos();
    uint32_t chunkLen = Swap4(m_pFileData->NextUI4());
    
    uint8_t pData = m_pFileData->GetAtCurrent();
    if (::memcmp(pData, "PACK", 4) == 0
        && pData[4] == -1 && pData[5] == -1
        && chunkLen > 16)
    {
        // runlen packing of chunk data -> unpack to buffer

        // update offset
        m_pFileData->SetCurrentPos(nPos +10);
       
        uint32_t unpackedLen = Swap4(m_pFileData->NextUI4());
        pData = new uint8_t[unpackedLen];
        
        // TODO: implement
        UnpackRunlen(m_pFileData->GetAtCurrent(), chunkLen, pData, unpackedLen);
        
    }
    else
    {
        // otherwise use as-is for further..
        m_pFileData->SetCurrentPos(nPos +4);
    }
    
    // process data, byteswap and such..
    switch (chunkID)
    {
    /*
    case CT_SONGDATA:
        break;
    case CT_SAMPLE:
        break;
    case CT_EMPTYSAMPLE:
        break;
    case CT_NOTEDATA:
        break;
    case CT_SAMPLENAMES:
        break;
    case CT_SEQUENCE:
        break;
    case CT_INFOTEXT:
        break;
    case CT_DELTASAMPLE:
        break;
    case CT_DELTA16:
        break;
    case CT_INFOTYPE:
        break;
    case CT_INFOOBJ:
        break;
    case CT_STRING:
        break;
        */
        
    default:
        bHandled = false;
        break;
    }
    
    nPos = m_pFileData->GetCurrentPos();
    if (bHandled == false)
    {
        // skip chunk if not supported..
        m_pFileData->SetCurrentPos(nPos + chunkLen);
    }
    return bHandled;
}


// TODO: implement
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
            // copy as-is next bytes (upto 255)
            uint8_t byteCount = pOrigData[nPos];
            nPos++;
            ::memcpy(pOutBuf + nOutPos, pOrigData + nPos, byteCount);
            nOutPos += byteCount;
            break;
            
        case 3:
            // upto 255 bytes of zero
            uint8_t byteCount = pOrigData[nPos];
            nPos++;
            ::memset(pOutBuf + nOutPos, 0, byteCount);
            nOutPos += byteCount;
            break;
        
        case 2:
            // 2x 32-bit long value
            uint32_t *pVal = (uint32_t*)(pOrigData + nPos);
            nPos += 4;
            uint32_t val = Swap4(*pVal);
            ::memcpy(pOutBuf + nOutPos, &val, 4);
            nOutPos += 4;
            ::memcpy(pOutBuf + nOutPos, &val, 4);
            nOutPos += 4;
            break;
            
        case 1:
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
bool CSymphoniePlayer::UnpackDelta()
{
    return false;
}


/////////// public

CSymphoniePlayer::CSymphoniePlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
{
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


