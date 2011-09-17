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


///////// adlib instrument format?

// conversion to usable format..
CAudioSample *CAdlibSample::ParseSample(uint8_t *pData, size_t nLen)
{
	CAudioSample *pSample = new CAudioSample();
	
	
	return pSample;
}


