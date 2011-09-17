//////////////////////////////////////
//
// ModPlayer.h
//
// abstract base class for each player implementation,
// give unified interface to application and share code easily
//
// Actually "decoder" would be more appropriate..
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef MODPLAYER_H
#define MODPLAYER_H

// use standard defines, fuck old typedefs
#include <stdint.h>

// needed here for CReadBuffer now..
#include "AnsiFile.h"

// pull this as well
#include "AudioSample.h"

// for decode/playback status and control,
// display and keeping track of where playback is..
#include "DecodeCtx.h"

// fwd. decl.
//class CReadBuffer;

class CModPlayer
{
protected:
    
    // simple buffer descriptor for reuse
    struct bufferedData_t
    {
        bufferedData_t()
        {
            m_pBuf = nullptr;
            m_nLen = 0;
        }
        
        /* fix use-cases first..
        ~bufferedData_t()
        {
            if (m_pBuf != nullptr)
            {
                delete m_pBuf;
                m_pBuf = nullptr;
            }
            m_nLen = 0;
        }
        */
        
        uint8_t *m_pBuf;
        size_t m_nLen;
    };
    
    
    // byteswap methods, needed everywhere,
    // can't inline when inherited so we lose one function call..
    //
	uint16_t Swap2(const uint16_t val) const
    {
        return (((val >> 8)) | (val << 8));
    }
	uint32_t Swap4(const uint32_t val) const
    {
        // swap bytes
        return (
                ((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
                ((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
                );
    }
	float SwapF(const float fval) const
    {
        // special case:
        // avoid implicit "float-int" conversion to avoid rounding errors,
        // cast via "bit-array" to another type.
        // (GCC-related note: change to union for byteswapping..)
        //
        float fTemp = fval;
        uint32_t tmp = Swap4((*((uint32_t*)(&fTemp))));
        fTemp = (*((float*)(&tmp)));
        return fTemp;
    }

    // make IFF-style tag from string
	uint32_t IFFTag(const char *buf) const
    {
		// note byteorder.. (little-endian CPU)
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
    
    // TODO: wrap for more cross-platform support?
    // (i.e. support build on both big/little endian systems)
    
    uint32_t ReadBEUI32()
    {
        // data: BE UI4
        uint32_t *pval = (uint32_t*)m_pFileData->GetNext(4);
        if (m_bBigEndian)
        {
            return (*pval);
        }
        return Swap4(*pval);
    }
    
    uint32_t ReadLEUI32()
    {
        // data: LE UI4
        uint32_t *pval = (uint32_t*)m_pFileData->GetNext(4);
        if (m_bBigEndian)
        {
            return Swap4(*pval);
        }
        return (*pval);
    }
    
    uint16_t ReadBEUI16()
    {
        // data: BE UI2
        uint16_t *pval = (uint16_t*)m_pFileData->GetNext(2);
        if (m_bBigEndian)
        {
            return (*pval);
        }
        return Swap2(*pval);
    }
    
    uint16_t ReadLEUI16()
    {
        // data: LE UI2
        uint16_t *pval = (uint16_t*)m_pFileData->GetNext(2);
        if (m_bBigEndian)
        {
            return Swap2(*pval);
        }
        return (*pval);
    }
    
    uint8_t ReadUI8()
    {
        // data: UI1
        return (m_pFileData->GetNext(1)[0]);
    }

    float ReadBEF32()
    {
        // data: BE F4
        float *pf = (float*)m_pFileData->GetNext(4);
        if (m_bBigEndian)
        {
            return (*pf);
        }
        return SwapF(*pf);
    }
    
    float ReadLEF32()
    {
        // data: LE F4
        float *pf = (float*)m_pFileData->GetNext(4);
        if (m_bBigEndian)
        {
            return SwapF(*pf);
        }
        return (*pf);
    }
    
    
    // original file data
    //
    CReadBuffer *m_pFileData;
    
    // implementation must allocate
    // in case of specifics..
    //
    DecodeCtx *m_pDecodeCtx;

    // use runtime-detection instead of compile-time?
    // (catch bi-endian machines where determined when booting..?)
    //
    bool m_bBigEndian;

    
public:
    CModPlayer(CReadBuffer *pFileData) 
        : m_pFileData(pFileData)
        , m_pDecodeCtx(nullptr)
        , m_bBigEndian(false)
    {
        // TODO: endianess detection at runtime 
        // instead of compile-time??
#ifdef BIGEND
        m_bBigEndian = true;
#else
        m_bBigEndian = false;
#endif
    }
    virtual ~CModPlayer() 
    {
        if (m_pDecodeCtx != nullptr)
        {
            delete m_pDecodeCtx;
            m_pDecodeCtx = nullptr;
        }
    }
    
    // start by determining details from metadata in header
    // before actual decoding (can help guessing buffer sizes etc.)
    virtual bool ParseFileInfo() = 0;
    
    // TODO: output format with QAudioFormat?
    // get suitable format for output
    // or use "forced" format?
    //virtual QAudioFormat GetOutputFormat() = 0;
    
    // initialize and setup decoding for playback:
    // set counters etc. and give descriptor of it to caller.
    //
    // TODO:? virtual DecodeCtx *PrepareDecoder(QAudioFormat &outputFormat) = 0;
    // or? virtual DecodeCtx *PrepareDecoder(QAudioDeviceInfo &device, QAudioFormat &outputFormat) = 0;
    // 
    // take shortcut for now..
	// implementation should initialize derived or default
	// instance of this..
	//virtual DecodeCtx *PrepareDecoder() = 0;
    virtual DecodeCtx *PrepareDecoder()
	{
		// use default implementation for now,
		// change back later..
		m_pDecodeCtx = new DecodeCtx();
		return m_pDecodeCtx;
	}
    
    // TODO: check details, "decode" in parts to buffer,
    // leave it upto caller to actually output..
    // player should not handle actual device for any hope of cross-platform support..
    //
    // note: so far expecting only PCM-encoded data to output 
    // as that is ubiquitous.. would be quite different to use something else,
    // like PDM or PWM which isn't commonly used..
    //
    //virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize) = 0;
    
    // also? playback-position slider control: give DecodeCtx* to caller?
    // -> no need if given init ?
    //virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize, DecodeCtx*) = 0;
};

#endif // MODPLAYER_H

