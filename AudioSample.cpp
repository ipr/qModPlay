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


///////// IFF 8SVX

#pragma pack(push, 1)

/* Can be more kinds in the future.	*/

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


bool CIff8svxSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	if (MakeTag("FORM") == ((pData[0] << 24)
							| (pData[1] << 16)
							| (pData[2] << 8)
							| (pData[3])));
	{
		nPos += 8; // skip
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
	if (MakeTag("FORM") == ((pData[0] << 24)
							| (pData[1] << 16)
							| (pData[2] << 8)
							| (pData[3])));
	{
		nPos += 8; // skip
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
			MaudHeader *pMhdr = (MaudHeader*)(pData + nPos);
			
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

bool CAiffSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	if (MakeTag("FORM") == ((pData[0] << 24)
							| (pData[1] << 16)
							| (pData[2] << 8)
							| (pData[3])));
	{
		nPos += 8; // skip
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

bool CWaveSample::ParseSample(uint8_t *pData, size_t nLen)
{
	size_t nPos = 0;

	// starts with full file header?
	if (MakeTag("RIFF") == ((pData[0] << 24)
							| (pData[1] << 16)
							| (pData[2] << 8)
							| (pData[3])));
	{
		nPos += 8; // skip
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
	
	MaestroHeader_t *pHdr = (MaestroHeader_t*)(pData + nPos);
}

///////// adlib instrument format?

// conversion to more usable format..
//CAudioSample *CAdlibSample::ParseSample(uint8_t *pData, size_t nLen)
bool CAdlibSample::ParseSample(uint8_t *pData, size_t nLen)
{
	return false;
}


