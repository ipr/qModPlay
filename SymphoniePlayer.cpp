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

// reduce duplications..
#include "FileType.h"

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
	// TODO:
	// implement something more generic like this:
	//CAudioSample *pSample = new CAudioSample();
	//pSample->fromDeltaPacked16bit();

    return false;
}

// TODO: implement
// unpack delta compression of sample data
bool CSymphoniePlayer::Decode16bitSample(const uint8_t *pData, const size_t nLen)
{
	// TODO:
	// implement something more generic like this:
	//CAudioSample *pSample = new CAudioSample();
	//pSample->fromDeltaPacked16bit();
	
    return false;
}

/*
SyMMSampleFormat CSymphoniePlayer::DetermineSampleFormat(uint8_t *pData, const size_t nLen)
{
	//SampleFormat enFmt = FMT_Unknown;
	if (::memcmp(pData, "MAESTRO", 7) == 0)
	{
		return FMT_Maestro;
	}
	if (::memcmp(pData, "16BT", 4) == 0)
	{
		return FMT_SYMPHEXPORT16BT;
	}
	if (::memcmp(pData, "RIFF", 4) == 0
		&& ::memcmp(pData + 8, "WAVE", 4) == 0)
	{
		return FMT_WAVE;
	}
	if (::memcmp(pData, "FORM", 4) == 0
		&& ::memcmp(pData + 8, "AIFF", 4) == 0)
	{
		return FMT_AIFF;
	}
	if (::memcmp(pData, "FORM", 4) == 0
		&& ::memcmp(pData + 8, "8SVX", 4) == 0)
	{
		return FMT_IFF8SVX;
	}
	
	// no header, just guess as raw-audio data..
	return FMT_Raw;
}
*/

// more generic sample format support
CAudioSample *CSymphoniePlayer::HandleSampleFormat(uint8_t *pData, const size_t nLen)
{
	CAudioSample *pSample = nullptr;
	
	// TODO: what is this type really?
	/*
	if (::memcmp(pData, "16BT", 4) == 0)
	{
		// symphonie 16-bit export
	}
	*/
	
	// expecting full file header in this case..
	CFileType type(pData, nLen);
	switch (type.m_enFileType)
	{
	case HEADERTYPE_8SVX:
		pSample = new CIff8svxSample();
		break;
	case HEADERTYPE_MAUD:
		pSample = new CIffMaudSample();
		break;
	case HEADERTYPE_AIFF:
		pSample = new CAiffSample();
		break;
	case HEADERTYPE_WAVE:
		pSample = new CWaveSample();
		break;
	case HEADERTYPE_MAESTRO:
		//pSample = new CMaestroSample();
		break;
		
	default:
		// raw audio sample or something entirely different?
		pSample = new CAudioSample();
		break;
	}
	
	pSample->ParseSample(pData, nLen);
	return pSample;
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
        bHandled = OnSongPositions(pData, chunkLen);
        break;
    case CT_SAMPLE: // -11
        // instrument sample
        bHandled = OnInstrumentSample(pData, chunkLen);
        break;
    case CT_SAMPLENAMES: // -14
        bHandled = OnInstrumentNames(pData, chunkLen);
        break;
    case CT_NOTEDATA: // -13
        // pattern data
        bHandled = OnPatternData(pData, chunkLen);
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
    
    m_nPatternSize = m_audioChannelcount*m_trackLength*4;
    if (m_nPatternSize == 0 || m_PatternData.m_nLen == 0)
    {
        // no data
        return false;
    }
    
    m_nPatternCount = m_PatternData.m_nLen / m_nPatternSize;
    if (m_nPatternCount == 0)
    {
        return false;
    }
    
    // TODO: some other stuff needed?
    
    return true;
}

// sample/instrument names
bool CSymphoniePlayer::OnInstrumentNames(uint8_t *pData, const size_t nLen)
{
    // ?? fixed size strings?
    int sampleNameCount = nLen / 256;
    uint8_t *pBuf = pData;
    
    // use value from CT_SAMPLENUMB, think this is correct way?
    m_pInstruments = new SyMMInstrument[m_instrumentCount];
    
    for (int i = 0; i < sampleNameCount; i++)
    {
        // should be null-terminated string
        m_pInstruments[i].m_name = (char*)pBuf;
        pBuf = (pBuf + m_pInstruments[i].m_name.length());
        
        if (m_pInstruments[i].m_name == "ViRT")
        {
            // virtual instrument, only visual information?
            m_pInstruments[i].m_bIsVirtual = true;
            m_pInstruments[i].m_number = i; // maybe some cross-reference needed..?
            m_pInstruments[i].m_type = (*pBuf);
            
        }
        else
        {
            // "normal" instrument
            m_pInstruments[i].m_bIsVirtual = false;
            m_pInstruments[i].m_number = i; // maybe some cross-reference needed..?
            m_pInstruments[i].m_type = (*pBuf);
            
        }
        
        pBuf = (pBuf + 256);
    }
    
    return true;
}

bool CSymphoniePlayer::OnInstrumentSample(uint8_t *pData, const size_t nLen)
{
	SyMMInstrument &instr = m_pInstruments[m_currentInstrument];
	
	// check what kind of sample-data there is
	//instr.m_sampleFormat.m_enSampleFormat = DetermineSampleFormat(pData, nLen);
	
	// more generic way to handle these..
	//instr.m_pSample = HandleSampleFormat(pData, nLen);
	
    return false;
}

bool CSymphoniePlayer::OnSequence(uint8_t *pData, const size_t nLen)
{
    uint8_t *pBuf = pData;
    
    m_sequenceCount = (nLen / SEQUENCE_SIZEOF);
    
    m_pSequences = new SyMMSequence[m_sequenceCount];

    for (int i = 0; i < m_sequenceCount; i++)
    {
        // some values are not read or padding?
        //size_t offset = SEQUENCE_SIZEOF*i;
        
        SyMMSequenceChunk_t *pSeq = (SyMMSequenceChunk_t*)pBuf;
        
        m_pSequences[i].m_startPosition = Swap2(pSeq->m_startPosition);
        m_pSequences[i].m_endPosition = Swap2(pSeq->m_endPosition);
        m_pSequences[i].m_action = Swap2(pSeq->m_action);
        m_pSequences[i].m_tune = Swap2(pSeq->m_tune);
        m_pSequences[i].m_loopCount = Swap2(pSeq->m_loopCount);
        
        // skip unused, is it just padding?
        // awful waste of space consider how tightly packed everything else is..
        //m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 6);
        
        pBuf = (pBuf + SEQUENCE_SIZEOF);
    }
    
    return true;
}

bool CSymphoniePlayer::OnSongPositions(uint8_t *pData, const size_t nLen)
{
    uint8_t *pBuf = pData;
    
    m_positionCount = (nLen / POSITION_SIZEOF);
    
    m_pPositions = new SyMMPosition[m_positionCount];

    for (int i = 0; i < m_positionCount; i++)
    {
        // some values are not read or padding?
        //size_t offset = POSITION_SIZEOF*i;
        
        SyMMSongPositionChunk_t *pSongPos = (SyMMSongPositionChunk_t*)pBuf;

        // is it always just one layer?
        // seems pointless to have following buffer then..
        m_pPositions[i].m_layerCount = 1; // ?
        /*
        // does this have to be a buffer
        // when single uint16 seems to be enough?
        m_pPositions[i].m_PatternNumbers.m_nLen = sizeof(uint16_t);
        m_pPositions[i].m_PatternNumbers.m_pBuf = new uint16_t[1];
        m_pPositions[i].m_PatternNumbers.m_pBuf[0] = Swap2(m_pFileData->NextUI2());
        */
        m_pPositions[i].m_nPatternNumber = Swap2(pSongPos->m_nPatternNumber);

        m_pPositions[i].m_tune = Swap2(pSongPos->m_tune);
        m_pPositions[i].m_startRow = Swap2(pSongPos->m_startRow);
        m_pPositions[i].m_rowLength = Swap2(pSongPos->m_rowLength);
        m_pPositions[i].m_loopCount = Swap2(pSongPos->m_loopCount);
        m_pPositions[i].m_speedCycl = Swap2(pSongPos->m_speedCycl);
        
        // skip unused, is it just padding?
        // awful waste of space consider how tightly packed everything else is..
        //m_pFileData->SetCurrentPos(m_pFileData->GetCurrentPos() + 20);
        
        pBuf = (pBuf + POSITION_SIZEOF);
    }
    
    return true;
}


/////////// public

CSymphoniePlayer::CSymphoniePlayer(CReadBuffer *pFileData)
    : CModPlayer(pFileData)
    , m_pInstruments(nullptr)
    , m_pSequences(nullptr)
    , m_sequenceCount(0)
    , m_pPositions(nullptr)
    , m_positionCount(0)
    , m_PatternData()
    , m_nPatternCount(0)
    , m_nPatternSize(0)
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
    delete [] m_pPositions;
    delete [] m_pSequences;
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
	
	// TODO: check if we can implement 
	// generic mixing routines in base-class,
	// for example: 
	// 256 tracker channels into 2 stereo output channels..
	//
	// (PCM-output, 2/4 output channels usually without surround in 8 channels..)
	
    return 0;
}

