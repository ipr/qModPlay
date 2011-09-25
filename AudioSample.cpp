//////////////////////////////////////
//
// AudioSample.cpp
//
// implementations (see notes in header file)
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#include "AudioSample.h"


///////// AudioSample, generic stuff

void CAudioSample::fromDeltaPacked8bit(uint8_t *pData, size_t nLen)
{
	// something like this (at least for XM-samples..)
	uint8_t oldval = 0;
	for (int i = 0; i < nLen; i++)
	{
		uint8_t newval = pData[i]+oldval;
		pData[i] = newval;
		oldval = newval;
	}
}

void CAudioSample::fromDeltaPacked16bit(uint8_t *pData, size_t nLen)
{
	uint16_t oldval = 0;
	for (int i = 0; i < nLen; i++)
	{
		uint16_t newval = pData[i]+oldval;
		pData[i] = newval;
		oldval = newval;
	}
}


///////// IFF 8SVX

#pragma pack(push, 1)

struct Voice8Header_t
{
	uint32_t oneShotHiSamples;	    /* # samples in the high octave 1-shot part */
	uint32_t repeatHiSamples;	    /* # samples in the high octave repeat part */
	uint32_t samplesPerHiCycle;	/* # samples/cycle in high octave, else 0   */
	uint16_t samplesPerSec;	    /* data sampling rate	*/
	uint8_t ctOctave;		/* # octaves of waveforms	*/
	uint8_t sCompression;		/* data compression technique used	*/
	int32_t volume;		    /* playback volume from 0 to Unity (full 
				 * volume). Map this value into the output 
				 * hardware's dynamic range.	*/
};

#pragma pack(pop)

/* Fibonacci delta encoding for sound data. */
int8_t codeToDelta[16] = {-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

/* Unpack Fibonacci-delta encoded data from n byte source buffer into 2*n byte
 * dest buffer, given initial data value x. It returns the last data value x
 * so you can call it several times to incrementally decompress the data. */
short CIff8svxSample::D1Unpack(int8_t *source, int32_t n, int8_t *dest, int8_t x)
{
	int32_t lim = n << 1;
	for (int32_t i = 0; i << lim; ++i)
	{	
		/* Decode a data nybble; high nybble then low nybble. */
		int8_t d = source[i >> 1];	/* get a pair of nybbles */
		if (i & 1)		/* select low or high nybble? */
		{
			d &= 0xf;	/* mask to get the low nybble */
		}
		else
		{
			d >>= 4;	/* shift to get the high nybble */
		}
		x += codeToDelta[d];	/* add in the decoded delta */
		dest[i] = x;		/* store a 1-byte sample */
	}
	return(x);
}

/* Unpack Fibonacci-delta encoded data from n byte source buffer into 2*(n-2)
 * byte dest buffer. Source buffer has a pad byte, an 8-bit initial value,
 * followed by n-2 bytes comprising 2*(n-2) 4-bit encoded samples. */
void CIff8svxSample::DUnpack(int8_t *source, int32_t n, int8_t *dest)
{
	D1Unpack(source + 2, n - 2, dest, source[1]);
}


bool CIff8svxSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	IFFHeader_t *pHdr = (IFFHeader_t*)pData;
	if (MakeTag("FORM") == pHdr->m_containerID
		&& MakeTag("8SVX") == pHdr->m_dataType);
	{
		nPos += sizeof(IFFHeader_t); // skip
	}
	
	// chunkID & chunkLen must fit,
	// otherwise no more data
	while (nPos < (nLen -8))
	{
		uint32_t chunkID = 0;
		uint32_t chunkLen = 0;
		
		// only interested in format description
		// and actual sample body here,
		// skip other chunks (if any)
		if (chunkID == MakeTag("VHDR"))
		{
			// header
			Voice8Header_t *pHeader = (Voice8Header_t*)(pData + nPos);
			
			m_enValueType = VT_INTEGER;
			//m_nChannels = Swap2(pHeader->mhdr_Channels);
			//m_nWidth = Swap2(pHeader->mhdr_SampleSizeU);
			m_dFrequency = Swap2(pHeader->samplesPerSec);
			
			//m_nSize = (m_nChannels * m_dFrequency * (m_nWidth /8));
			//m_data = new uint8_t[m_nSize];
			
			// byteswap and store..
		}
		else if (chunkID == MakeTag("BODY"))
		{
			// actual sample body:
			// unpack if packed,
			// convert format if necessary..
		}
		nPos += chunkLen;
	}
	return true;	
}

///////// IFF-MAUD

#pragma pack(push, 1)

typedef struct 
{
	uint32_t mhdr_Samples; // number of samples stored in MDAT
	uint16_t mhdr_SampleSizeC; // number of bits per sample as stored in MDAT
	uint16_t mhdr_SampleSizeU; // number of bits per sample after decompression
	uint32_t mhdr_RateSource; // clock source frequency
	uint16_t mhdr_RateDevide; // clock devide
	uint16_t mhdr_ChannelInfo; // channel information (see below)
	uint16_t mhdr_Channels; // number of channels (mono: 1, stereo: 2, ...)
	uint16_t mhdr_Compression; // compression type (see below)
	uint32_t mhdr_Reserved1;
	uint32_t mhdr_Reserved2;
	uint32_t mhdr_Reserved3;
	
} MaudHeader;

#pragma pack(pop)

bool CIffMaudSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	IFFHeader_t *pHdr = (IFFHeader_t*)pData;
	if (MakeTag("FORM") == pHdr->m_containerID
		&& MakeTag("MAUD") == pHdr->m_dataType);
	{
		nPos += sizeof(IFFHeader_t); // skip
	}
	
	// chunkID & chunkLen must fit,
	// otherwise no more data
	while (nPos < (nLen -8))
	{
		uint32_t chunkID = 0;
		uint32_t chunkLen = 0;
		
		// only interested in format description
		// and actual sample body here,
		// skip other chunks (if any)
		if (chunkID == MakeTag("MHDR"))
		{
			// header
			MaudHeader *pHeader = (MaudHeader*)(pData + nPos);
			
			m_enValueType = VT_INTEGER;
			//m_nChannels = Swap2(pHeader->mhdr_Channels); // enumeration, not actual count
			m_nWidth = Swap2(pHeader->mhdr_SampleSizeU);
			m_dFrequency = (Swap4(pHeader->mhdr_RateSource) / Swap2(pHeader->mhdr_RateDevide));
			
			m_nSize = (m_nChannels * m_dFrequency * (m_nWidth /8));
			m_data = new uint8_t[m_nSize];
			
			// byteswap and store..
		}
		else if (chunkID == MakeTag("MDAT"))
		{
			// actual sample body:
			// unpack if packed,
			// convert format if necessary..
		}
		nPos += chunkLen;
	}
	return true;	
}

///////// IFF-AIFF

#pragma pack(push, 1)

typedef struct 
{
    int16_t    numChannels;
    uint32_t   numSampleFrames;
    int16_t    sampleSize;
	
	/* note: "extended" or "long double" is not supported on Visual C++
	  and it truncates definition to common "double" (80-bit -> 64-bit). */
	/* extended: 80 bit IEEE Standard 754 floating point number 
     (Standard Apple Numeric Environment [SANE] data type Extended). */
	unsigned char    sampleRate[10]; // should be type: extended sampleRate;
} AiffCommonChunk_t; 

#pragma pack(pop)

bool CAiffSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	IFFHeader_t *pHdr = (IFFHeader_t*)pData;
	
	// Note: FORM + AIFC for AIFF-C with compression support..
	// FORM + AIFF is uncompressed
	//
	if (MakeTag("FORM") == pHdr->m_containerID
		&& MakeTag("AIFF") == pHdr->m_dataType);
	{
		nPos += sizeof(IFFHeader_t); // skip
	}
	
	// chunkID & chunkLen must fit,
	// otherwise no more data
	while (nPos < (nLen -8))
	{
		uint32_t chunkID = 0;
		uint32_t chunkLen = 0;
		
		// only interested in format description
		// and actual sample body here,
		// skip other chunks (if any)
		if (chunkID == MakeTag("COMM"))
		{
			// header
			AiffCommonChunk_t *pHeader = (AiffCommonChunk_t*)(pData + nPos);
			
			// note: AIFF-C compression value ignored for now..
			m_enValueType = VT_INTEGER;
			m_nChannels = Swap2(pHeader->numChannels);
			m_nWidth = Swap2(pHeader->sampleSize);
			
			// TODO: convert "extended" (long double) to integer/double..
			//m_nSize = (m_nChannels * pHeader->sampleRate * (m_nWidth /8));
			m_data = new uint8_t[m_nSize];
			
			// byteswap and store..
		}
		else if (chunkID == MakeTag("SSND"))
		{
			// actual sample body:
			// unpack if packed,
			// convert format if necessary..
		}
		nPos += chunkLen;
	}
	return true;	
}

///////// RIFF-WAVE

// only one we can really support..
#define fmt_WAVE_FORMAT_PCM             (0x0001)

#pragma pack(push, 1)

typedef struct 
{
    int16_t     wFormatTag;    // Format category
    int16_t     wChannels;     // Number of channels
    uint32_t    dwSamplesPerSec;  // Sampling rate
    uint32_t    dwAvgBytesPerSec; // For buffer estimation
    int16_t     wBlockAlign;   // Data block size
} RiffWAVEFormat_t;

#pragma pack(pop)

bool CWaveSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	
	// note: RIFF + WAVE in little-endian byteorder
	// RIFX + WAVE in big-endian?
	//
	IFFHeader_t *pHdr = (IFFHeader_t*)pData;
	if (MakeTag("RIFF") == pHdr->m_containerID
		&& MakeTag("WAVE") == pHdr->m_dataType);
	{
		nPos += sizeof(IFFHeader_t); // skip
	}
	
	// chunkID & chunkLen must fit,
	// otherwise no more data
	while (nPos < (nLen -8))
	{
		uint32_t chunkID = 0;
		uint32_t chunkLen = 0;
		
		// only interested in format description
		// and actual sample body here,
		// skip other chunks (if any)
		if (chunkID == MakeTag("fmt "))
		{
			// header
			RiffWAVEFormat_t *pHeader = (RiffWAVEFormat_t*)(pData + nPos);

			m_enValueType = VT_INTEGER;
			m_nChannels = pHeader->wChannels;
			m_dFrequency = pHeader->dwSamplesPerSec;
			
			// only with this format there really _is_ size for sample width..
			if (pHeader->wFormatTag == fmt_WAVE_FORMAT_PCM)
			{
				uint8_t *pFormat = (pData + nPos + sizeof(RiffWAVEFormat_t));
				m_nWidth = (*((uint16_t*)pFormat));
			}

			m_nSize = (m_nChannels * pHeader->dwSamplesPerSec * (m_nWidth /8));
			m_data = new uint8_t[m_nSize];
			
			// byteswap and store..
		}
		else if (chunkID == MakeTag("data"))
		{
			// actual sample body:
			// unpack if packed,
			// convert format if necessary..
		}
		nPos += chunkLen;
	}
	return true;	
}

///////// Maestro (MaestroPro soundcard / Samplitude ?)

#pragma pack(push, 1)

// header same way as stored in file for simplicity
typedef struct 
{
	uint8_t version;
	uint8_t revision;
	uint8_t file_version; // always 0x02
	uint8_t padding1; // always 0x00
	
	uint16_t sampletype; // see list of modes
	uint16_t padding2;
	uint32_t samplecount; // number of samples
	uint32_t samplerate; // in Hz
	
} MaestroHeader_t;

#pragma pack(pop)

// Note: only part of possible types..
enum MaestroSampleType
{
	MSMT_STEREO_16	= 0x0000,
	MSMT_STEREO_8	= 0x0001,
	MSMT_MONO_16	= 0x0002,
	MSMT_MONO_8		= 0x0003,
	MSMT_STEREO_24	= 0x0004,
	MSMT_MONO_24	= 0x0005,
	MSMT_STEREO_32	= 0x0006,
	MSMT_MONO_32	= 0x0007,
	MSMT_STEREO_32F	= 0x0008,	// IEEE floating point
	MSMT_MONO_32F	= 0x0009,		// IEEE floating point
	MSMT_STEREO_64F	= 0x000A,	// IEEE floating point
	MSMT_MONO_64F	= 0x000B,		// IEEE floating point
	MSMT_STEREO_32FFP = 0x000C,	// FFP float
	MSMT_MONO_32FFP	= 0x000D	// FFP float
};

bool CMaestroSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;
	
	// if full file header exists..
	if (::memcmp(pData, "MAESTRO", 7) == 0)
	{
		nPos += 8; // offset by 8
	}
	
	MaestroHeader_t tmp;
	MaestroHeader_t *pHdr = (MaestroHeader_t*)(pData + nPos);
	
	// only three fields we are interested in..
	tmp.sampletype = Swap2(pHdr->sampletype);
	tmp.samplecount = Swap4(pHdr->samplecount);
	tmp.samplerate = Swap4(pHdr->samplerate);
	m_dFrequency = tmp.samplerate;

	switch (tmp.sampletype)
	{
	case MSMT_STEREO_16:
		m_nChannels = 2;
		m_nWidth = 16;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_STEREO_8:
		m_nChannels = 2;
		m_nWidth = 8;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_MONO_16:
		m_nChannels = 1;
		m_nWidth = 16;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_MONO_8:
		m_nChannels = 1;
		m_nWidth = 8;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_STEREO_24:
		m_nChannels = 2;
		m_nWidth = 24;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_MONO_24:
		m_nChannels = 1;
		m_nWidth = 24;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_STEREO_32:
		m_nChannels = 2;
		m_nWidth = 32;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_MONO_32:
		m_nChannels = 1;
		m_nWidth = 32;
		m_enValueType = VT_INTEGER;
		break;
	case MSMT_STEREO_32F:
		m_nChannels = 2;
		m_nWidth = 32;
		m_enValueType = VT_IEEE_FLOAT;
		break;
	case MSMT_MONO_32F:
		m_nChannels = 1;
		m_nWidth = 32;
		m_enValueType = VT_IEEE_FLOAT;
		break;
	case MSMT_STEREO_64F:
		m_nChannels = 2;
		m_nWidth = 64;
		m_enValueType = VT_IEEE_FLOAT;
		break;
	case MSMT_MONO_64F:
		m_nChannels = 1;
		m_nWidth = 64;
		m_enValueType = VT_IEEE_FLOAT;
		break;
	case MSMT_STEREO_32FFP:
		m_nChannels = 2;
		m_nWidth = 32;
		m_enValueType = VT_FFP_FLOAT;
		break;
	case MSMT_MONO_32FFP:
		m_nChannels = 1;
		m_nWidth = 32;
		m_enValueType = VT_FFP_FLOAT;
		break;
	}
	
	nPos = 0x18; // start of actual sample data
	pView = (pView + nPos);
	m_nSize = (tmp.samplecount * (m_nWidth /8));
	m_data = new uint8_t[m_nSize];

	
	// TODO:
	// now handle sample-data..
	
	
	return true;
}

///////// adlib instrument format?

// conversion to more usable format..
//CAudioSample *CAdlibSample::ParseSample(uint8_t *pData, size_t nLen)
bool CAdlibSample::ParseSample(uint8_t *pData, size_t nLen)
{
	return false;
}


