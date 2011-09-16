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
    //
    virtual bool ParseSample(uint8_t *pData, size_t nLen) { return true; }
};


///////// IFF 8SVX

// something like this..?
//
class CIff8svxSample : public CAudioSample
{
protected:
    //Voice8Header m_VoiceHeader; // VHDR

public:
    CIff8svxSample()
        : CAudioSample()
        , m_VoiceHeader()
    {}
    ~CIff8svxSample()
    {}
    
    // from file/buffer
    //virtual bool ParseSample(uint8_t *pData, size_t nLen);
};


#endif // AUDIOSAMPLE_H
