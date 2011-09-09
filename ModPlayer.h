//////////////////////////////////////
//
// ModPlayer.h
//
// abstract base class for each player implementation,
// give unified interface to application and share code easily
//
// Actually "decoder" would be more appropriate..
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef MODPLAYER_H
#define MODPLAYER_H

// use standard defines, fuck old typedefs
#include <stdint.h>

// fwd. decl.
class CReadBuffer;

class CModPlayer
{
protected:
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

    
    // original file data
    CReadBuffer *m_pFileData;
    
public:
    CModPlayer(CReadBuffer *pFileData) 
        : m_pFileData(pFileData)
    {}
    virtual ~CModPlayer() {}
    
    // start by determining details from metadata in header
    // before actual decoding (can help guessing buffer sizes etc.)
    virtual bool ParseFileInfo() = 0;
    
    // TODO: output format with QAudioFormat?
    // get suitable format for output
    // or use "forced" format?
    //virtual QAudioFormat GetOutputFormat() = 0;
    
    // TODO: check details, "decode" in parts to buffer,
    // leave it upto caller to actually output..
    //virtual size_t Decode(void *pBuffer, const size_t nBufSize) = 0;
    
};

#endif // MODPLAYER_H

