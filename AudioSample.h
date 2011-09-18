//////////////////////////////////////
//
// AudioSample.h
//
// abstract base class for audio sample information,
// give unified interface/reusable code for
// module information processing.
//
// large part of samples may be generated audio
// based on notes&effects so that needs to be supported..
//
// also modules can have different "recorded" sample types
// so just try to make reusable and allow differences,
// actual playback is module-type dependant anyway..
//
// derived classes would implement something like these:
// - IFF 8SVX
// - 16-bit IFF?
// - AIFF?
// - RIFF-WAVE?
//
// main guideline is that CAudioSample (or derived)
// holds PCM-encoded sampledata,
// which can be mixed or otherwise used on actual output.
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef AUDIOSAMPLE_H
#define AUDIOSAMPLE_H

// use standard defines, fuck old typedefs
#include <stdint.h>

// fwd. decl.
//class CReadBuffer;

/////// base "interface"
// 
class CAudioSample
{
protected:
    
    // actual data, note bit-width..
    uint8_t *m_data;
    
    size_t m_nSize; // size in bytes
    size_t m_nWidth; // bit-width of sample (8-bit/16-bit..32-bit)
    size_t m_nLength; // (m_nSize / (m_nWidth/8)) ?

    // note: this means "audio-channels"
    // and not "tracker-channels" which may be different
    // when outputting e.g. 256 channels in just two stereo-channels..
    size_t m_nChannels; // needed if we have PCM-encoded data here..?
        
    double m_dFrequency; // sampling rate (such as 44.1kHz)
    double m_dDuration; // sample duration (in millisec? microsec?)
    
	
	// argh! hate copying but here goes..
	// figure out sharing later..
	
	uint16_t Swap2(const uint16_t val) const
    {
        return (((val >> 8)) | (val << 8));
    }
	uint32_t Swap4(const uint32_t val) const
    {
        return (
                ((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
                ((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
                );
    }
	float SwapF(const float fval) const
    {
        float fTemp = fval;
        uint32_t tmp = Swap4((*((uint32_t*)(&fTemp))));
        fTemp = (*((float*)(&tmp)));
        return fTemp;
    }
	
	
	uint32_t MakeTag(const unsigned char *buf) const
    {
		// note byteorder.. (little-endian CPU)
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
	
public:
    CAudioSample()
        : m_data(nullptr)
    {}
    ~CAudioSample()
    {
        delete m_data;
    }
    
    // sample "width" (bit-size)
    void setWidth(size_t nWidth)
    {
        m_nWidth = nWidth;
    }
    
    // sampling rate / frequency
    void setFrequency(double dFrequency)
    {
         m_dFrequency = dFrequency;
    }
    
    // for pre-generating sample data
    // which could be modulated/effected on playback?
    //
    // may be based on ProTracker period table for example..
    //
    virtual void GenerateSample() {}
    
    // for processing "recorded" sample
    // from file/buffer
    // (check)
    virtual bool ParseSample(uint8_t *pData, size_t nLen) { return true; }
};


///////// IFF 8SVX

// something like this..?
//
class CIff8svxSample : public CAudioSample
{
protected:
    //Voice8Header m_VoiceHeader; // VHDR

	//void unpackRunlen(uint8_t *pData, size_t nLen);
	//void unpackDeltapack(uint8_t *pData, size_t nLen);
	
public:
    CIff8svxSample()
        : CAudioSample()
    {}
    ~CIff8svxSample()
    {}
    
    // from file/buffer,
	// conversion if sample-format is alreay set?
    virtual bool ParseSample(uint8_t *pData, size_t nLen);
};

///////// IFF-MAUD

class CIffMaudSample : public CAudioSample
{
};

///////// IFF-AIFF

class CAiffSample : public CAudioSample
{
};

///////// RIFF-WAVE

class CWaveSample : public CAudioSample
{
};

///////// Maestro? (Samplitude?)

class CMaestroSample : public CAudioSample
{
};

///////// adlib instrument format?

class CAdlibSample : public CAudioSample
{
public:
	CAdlibSample()
        : CAudioSample()
    {}
    ~CAdlibSample()
    {}
    
    // from file/buffer,
	// conversion if sample-format is alreay set?
    static CAudioSample *ParseSample(uint8_t *pData, size_t nLen);
};


#endif // AUDIOSAMPLE_H
