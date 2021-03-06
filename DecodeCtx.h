//////////////////////////////////////
//
// DecodeCtx.h
//
// base class for audio decode/playback
// status and control,
// to use with decoder control and status.
//
// gives unified interface across formats
// and decoders.
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef DECODECTX_H
#define DECODECTX_H

#include <stdint.h>

// TODO: in progress..
// allow deriving for implementations specifics..
//
// also use internally in decoder/playback handler
// to keep track of status and positions.
//
class DecodeCtx
{
protected:
    
    // TODO: check for necessary helpers during playback..
    
    // could use looping normally
    // but must disable for dumping to a file..?
    // -> user interface option must control..
    bool m_bIsLoopingPlayback;
    
    // current frame position in decode/playback
    uint64_t m_nCurrentFrame;
    
    // decode/playback frame size:
    // channels * (samplesize/8) 
    // -> single frame in bytes
    size_t m_nFrameSize;

    // something close to total length/size of output?
    // frequency*framesize?
    //double m_dTotalSize;
    
    // count of frames:
    // filesize / framesize
    // TODO: drop this, won't know this in all formats
    // until counting how much played (looping etc.)
    //uint64_t m_nFrameCount;
    
	// these are output-format description,
	// tracker may have lots more channels
	size_t m_nSampleSize;
	size_t m_nChannelCount;
	size_t m_nSampleRate;
	
    // frames per second?
	//
    // bytes per second:
    // filesize / (framesize * sample rate)
	// not needed?
    //double m_dBytesPerSecond;
    
    // duration of single frame in microseconds:
    // device-independent output resolution
    double m_dFrameDuration;
	
	// TODO: pattern-row counter here also?
	// for player/decode needs only..?
	// or display also?
	size_t m_nCurrentPattern;
	size_t m_nCurrentTrack;
	size_t m_nCurrentSubsong;
	// ..

    // channel count: amount of channels per frame
    // sample size: sample width in bits    
	/*
    void setFrameSize(const size_t nChannels, const size_t nSampleSize)
    {
        m_nFrameSize = nChannels * (nSampleSize/8);
    }
	*/
    /*
    // drop this..
    void setFrameCount(const uint64_t fileSize)
    {
        m_nFrameCount = (fileSize / m_nFrameSize);
    }
    */
    // for simplistic buffer estimate,
    // sample rate in Hz
	/* not needed?
    void setByteRate(const double dSampleRate)
    {
        m_dBytesPerSecond = (m_nFrameSize * dSampleRate);
    }
	*/
    
    //
    // TODO: helper conversions?
    // some mods have stuff like VBlank/VSync value for timing
    // which need to be converted for playback on entirely different hardware..
    // some use stuff like CPU jiffies/ticks for timing values..
    // need some cross-platform timing support such that
    // frameduration = (base-frequency / dividend) .. where duration is some suitable unit..
    // e.g. dur = 50.0MHz / 2 -> 25.0MHz ~25us
    //
    // Also, when timer interrupts occur at programmable rate,
    // we need to know that rate for cases..
    //
    // Example: AHX format uses Amiga CIA chip speed as base frequency
    // and timer-value is stored as multiplier of that base..
    // -> convert into frame-time
    //
    // In another case divisor is based 
    // on PAL/NTSC vertical blank/vertical sync timings..
    // (usually tempo is defined for certain blankrate only)
    // -> convert into frame-time
    // 
    /*
    double toMicrosec(float baseFrequency, int divisor)
    {
        // something like this needed?
        return (double)(baseFrequency / divisor);
    }
    */
    
    // the usual framerates..
    // additional scan time also?
    // note: sync needed still..?
    /*
    enum DisplayFrameRate
    {
        PAL = 24,
        PALHI = 25, // 
        NTSC = 
    };
    */
    
    // here framerate can be non-standard for exceptional cases..
    /*
    double toMicrosec(int frameRate, int divisor)
    {
        double d = (1/frameRate);
        // also some scan-time needed..?
        return (d/divisor);
    }
    */
    
public:
    DecodeCtx() 
        : m_bIsLoopingPlayback(false)
        , m_nCurrentFrame(0)
        , m_nFrameSize(0)
        //, m_nFrameCount(0)
        //, m_dBytesPerSecond(0)
        , m_dFrameDuration(0)
		, m_nSampleSize(0)
		, m_nChannelCount(0)
		, m_nSampleRate(0)
    {}

    DecodeCtx(const size_t nChannels, const size_t nSampleSize, const size_t nSampleRate) 
        : m_bIsLoopingPlayback(false)
        , m_nCurrentFrame(0)
        , m_nFrameSize(0)
        , m_dFrameDuration(0)
		, m_nSampleSize(nSampleSize)
		, m_nChannelCount(nChannels)
		, m_nSampleRate(nSampleRate)
    {}
    
    // set initial values for decoder,
    // note that currently expecting all output to happen in PCM-encoded data
    // so some simplifications exists..
    //
    // expecting parameters:
    // - channel count in output (two for stereo, >2 for surround-like formats..)
    // - sample width in bits for output (usually 8 at minimum, 16 likely, maybe even 24..?)
    // - sample rate in Hz
    // 
    void initialize(const size_t nChannels, const size_t nSampleSize, const size_t nSampleRate)
    {
		m_nSampleSize = nSampleSize;
		m_nChannelCount = nChannels;
		m_nSampleRate = nSampleRate;
		
		// one sample from each channel in a single frame
		// (output channel)
		m_nFrameSize = nChannels * (nSampleSize/8);
		
        //setFrameSize(nChannels, nSampleSize);
        //setFrameCount(fileSize);
        //setByteRate(dSampleRate);
    }
	
	// audio-frame duration in microsec (or millisec..?)	
	double frameduration()
	{
		return m_dFrameDuration;
	}
	
	// size of single audio-frame for buffer counting
	size_t frameSize()
	{
		return m_nFrameSize;
	}

	// temp until something better shows up..
	size_t sampleSize()
	{
		return m_nSampleSize;
	}
	size_t channelCount()
	{
		return m_nChannelCount;
	}
	size_t sampleRate()
	{
		return m_nSampleRate;
	}
	
    // set values to start (same as update(0))
    void setBegin()
    {
        m_nCurrentFrame = 0;
    }
    
    // step forwards n frames
    void stepFwd(int64_t i64Count = 0)
    {
        /*
        if ((m_nCurrentFrame + i64Count) < m_nFrameCount)
        {
            m_nCurrentFrame += i64Count;
        }
        else
        {
            // just set to end
            m_nCurrentFrame = m_nFrameCount;
        }
        */
    }
    
    // step backwards n frames
    void stepBck(int64_t i64Count = 0)
    {
        if (m_nCurrentFrame > i64Count)
        {
            m_nCurrentFrame -= i64Count;
        }
        else
        {
            // just set to start
            m_nCurrentFrame = 0;
        }
    }
    
    // set values to start (same as update(length()))
    void setEnd()
    {
        //m_nCurrentFrame = m_nFrameCount;
    }
    
    // force absolute position (keep track of playback)
    void updatePosition(const int64_t i64Pos)
    {
        m_nCurrentFrame = i64Pos;
    }
    
    // get current frame position
    uint64_t position()
    {
        return m_nCurrentFrame;
    }
    
    // total length in frames
    /*
    uint64_t length()
    {
        return m_nFrameCount;
    }
    */
    
    // allow direct access in playback/decoder implementation..
    friend class CModPlayer;
};


#endif // DECODECTX_H
