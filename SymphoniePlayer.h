//////////////////////////////////////
//
// CSymphoniePlayer :
// rewrite of SymMOD player in (mostly) portable C++
//
// Based on M68k assembler and Java source codes..
// (well, at least there _are_ sources..)
//
// Just interface for now..
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef SYMPHONIEPLAYER_H
#define SYMPHONIEPLAYER_H

#include "ModPlayer.h"

class CSymphoniePlayer : public CModPlayer
{
protected:
    
    // note:
    //
    enum ChunkType
    {
        CT_CHANNELNUMB  = 0xFFFFFFFF,   // -1
        CT_TRACKLEN     = 0xFFFFFFFE,   // -2
        CT_PATTERNNUMB  = 0xFFFFFFFD,   // -3
        CT_SAMPLENUMB   = 0xFFFFFFFC,   // -4
        CT_NOTESIZE     = 0xFFFFFFFB,   // -5
        CT_SYSTEMSPEED  = 0xFFFFFFFA,   // -6
        CT_ISSONG       = 0xFFFFFFF9,   // -7
        //CT_ = 0xFFFFFFF8,         // not used -> break if found..
        //CT_ = 0xFFFFFFF7,         // not used -> break if found..
        CT_SONGDATA = 0xFFFFFFF6,       // -10
        CT_SAMPLE = 0xFFFFFFF5,         // -11
        CT_EMPTYSAMPLE = 0xFFFFFFF4,    // -12 no data ? empty chunk? single long??
        CT_NOTEDATA = 0xFFFFFFF3,       // -13 
        CT_SAMPLENAMES = 0xFFFFFFF2,    // -14
        CT_SEQUENCE = 0xFFFFFFF1,       // -15
        CT_INFOTEXT = 0xFFFFFFF0,       // -16
        CT_DELTASAMPLE = 0xFFFFFFEF,    // -17 deltapacked sample (8-bit?)
        CT_DELTA16 = 0xFFFFFFEE,        // -18 16-bit deltapacked sample
        CT_INFOTYPE = 0xFFFFFFED,       // -19
        CT_INFOOBJ = 0xFFFFFFEC,        // -20
        CT_STRING = 0xFFFFFFEB,         // -21
        
        CT_NGSAMPLEBOOST = 0x0000000A,  // 10
        CT_PITCHDIFF = 0x0000000B,      // 11
        CT_SAMPLEDIFF = 0x0000000C,     // 12
        
        CT_EOF = 0x00000000             // end of file
    };
    
    // values of single-value parameters
    int32_t m_audioChannelcount;
    int32_t m_trackLength;
    int32_t m_totalPatternlength;
    int32_t m_instrumentCount;
    int32_t m_songEventSize;
    int32_t m_systemBPM;
    int32_t m_isPureSong;
    
    int32_t m_globalSampleBoost;
    int32_t m_stereoEncPitchDiff;
    int32_t m_stereoEncSampleDiff;

    // always 1..?
    long m_lVersion;
    
    bool OnChunk(uint32_t chunkID);
    bool OnLargeChunk(uint32_t chunkID);

	// TODO:..
	//bool UnpackRunlen();
    
public:
    CSymphoniePlayer(CReadBuffer *pFileData);
    virtual ~CSymphoniePlayer() {}
    
    virtual bool ParseFileInfo();
};

#endif // SYMPHONIEPLAYER_H
