//////////////////////////////////////
//
// AudioSample.h
//
// abstract base class for audio sample information,
// give unified interface/reusable code for
// module information processing.
//
// modules can have different sample types
// so just try to make reusable and allow differences,
// actual playback is module-type dependant anyway..
//
// derived classes would implement something like these:
// - IFF 8SVX
// - 16-bit IFF?
// - AIFF?
// - RIFF-WAVE?
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef AUDIOSAMPLE_H
#define AUDIOSAMPLE_H

// use standard defines, fuck old typedefs
#include <stdint.h>

// fwd. decl.
class CReadBuffer;

class CAudioSample
{
public:
    CAudioSample()
    {}
    ~CAudioSample()
    {}

    virtual bool ParseSample(uint8_t *pData, size_t nLen) = 0;
};

#endif // AUDIOSAMPLE_H
