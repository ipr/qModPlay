//////////////////////////////////////
//
// CSymphoniePlayer :
// player for Symphonie modules (SymMOD)
// in (mostly) portable C++
//
// Based on M68k assembler and Java source codes 
// by: Patrick Meng 2008
// 
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//

#ifndef SYMPHONIEPLAYER_H
#define SYMPHONIEPLAYER_H

#include "ModPlayer.h"

#include <string>


// sample formats supported in instruments
/*
enum SyMMSampleFormat
{
	FMT_Unknown = 0,
    FMT_Maestro = 1,
    FMT_WAVE = 2,
    FMT_AIFF = 3,
    FMT_Raw = 4,
    FMT_IFF8SVX = 5,
    FMT_SYMPHEXPORT16BT = 6,
	FMT_IFFMAUD = 7 // ? 16-bit IFF ?
};
*/

// TODO: this could kindof stuff
// should be a part of CAudioSample in future..
/*
struct SyMMSampleInfo_t
{
	//SyMMSampleFormat m_enSampleFormat;
	size_t m_nBitWidth;
	
	// constructor
	SyMMSampleInfo_t()
	{
		//m_enSampleFormat = FMT_Unknown;
		m_nBitWidth = 0;
	}
};
*/

// force 1-byte alignment for struct (no padding)
#pragma pack(push, 1)

struct SyMMSequenceChunk_t
{
    uint16_t m_startPosition;     // 
    uint16_t m_endPosition;       // 
    uint16_t m_action;            // 
    uint16_t m_tune;              // 
    uint16_t m_loopCount;         // 
};

struct SyMMSongPositionChunk_t
{
    uint16_t m_nPatternNumber;     // 
    uint16_t m_tune;               // 
    uint16_t m_startRow;           // 
    uint16_t m_rowLength;          // 
    uint16_t m_loopCount;          // 
    uint16_t m_speedCycl;          // 
};


#pragma pack(pop)


class CSymphoniePlayer : public CModPlayer
{
protected:
    
    // these just offsets in file used when parsing?
    // -> could skip..
    enum SymphonieSequence 
    { 
        SEQUENCE_STARTPOS	=	0,
        SEQUENCE_LENGTH		=	2,
        SEQUENCE_LOOP		=	4,	//;1 = 1xLOOPEN
        SEQUENCE_INFO		=	6,	//;0=PLAY 1=SKIP -1=LAST OF SONG
        SEQUENCE_TUNE		=	8,	//;SEQ TRANSPOSE HALBTONES
    
        // padding or unused variables here?
        SEQUENCE_SIZEOF		=	16,
    
        SEQINFO_PLAY		=	0,
        SEQINFO_SKIP		=	1,
        SEQINFO_ENDSONG		=	-1
    };

    enum SymphoniePosition 
    {
        POSITION_LOOPNUMB       =   4,
        POSITION_LOOPCOUNT      =   6,
        POSITION_PATNUM         =   8,
        POSITION_STARTPOINT     =   10,
        POSITION_LEN            =   12,
        POSITION_SPEED          =	14,
        POSITION_TUNE           =	16,
        POSITION_EVENTSPERLINE	=	18,	//;WORD: 0=1 Event per Line
        POSITION_SIZEOF         =	32
    };
    
    // Offsets to Event
    enum SymphonieNote
    { 
        NOTE_FX     = 0,
        NOTE_PITCH  = 1,
        NOTE_VOLUME = 2,
        NOTE_INSTR  = 3
    };

    // Event Types as in SymphonieNote.NOTE_FX
    enum SymphonieEventType 
    {
        FX_KEYON            =   0,
        FX_VOLUMESLIDEUP    =   1,
        FX_VOLUMESLIDEDOWN  =   2,
        FX_PITCHSLIDEUP     =   3,
        FX_PITCHSLIDEDOWN   =   4,
        FX_REPLAYFROM       =   5,
        FX_FROMANDPITCH     =   6,
        FX_SETFROMADD       =   7,
        FX_FROMADD          =   8,
        FX_SETSPEED         =   9,
        FX_ADDPITCH         =   10,
        FX_ADDVOLUME        =   11,
        FX_VIBRATO          =   12,
        FX_TREMOLO          =   13,
        FX_SAMPLEVIB        =   14,
        FX_PSLIDETO         =   15,
        FX_RETRIG           =   16,
        FX_EMPHASIS         =   17,
        FX_ADDHALVTONE      =   18,
        FX_CV               =   19,
        FX_CVADD            =   20,
        FX_FILTER           =   23,  
        FX_DSPECHO          =   24, // Echo is defined as repeating delays
        FX_DSPDELAY         =   25, // Delay
        FX_DSPCHOR          =   26  // Chorus Experimental
    };
    
    enum SymphonieVolume 
    {
        VOLUME_STOPSAMPLE	=	254,
        VOLUME_CONTSAMPLE	=	253,
        VOLUME_STARTSAMPLE	=	252,
        VOLUME_KEYOFF		=	251,
        VOLUME_SPEEDDOWN	=	250,
        VOLUME_SPEEDUP		=	249,
        VOLUME_SETPITCH		=	248,
        VOLUME_PITCHUP		=	247,
        VOLUME_PITCHDOWN	=	246,
        VOLUME_PITCHUP2		=	245,
        VOLUME_PITCHDOWN2	=	244,
        VOLUME_PITCHUP3		=	243,
        VOLUME_PITCHDOWN3	=	242,
        VOLUME_NONE         =	0,
        VOLUME_MIN          =	1,
        VOLUME_MAX          =	100,
        VOLUME_COMMAND		=	200
    };
    
    enum SymphonieNotePitch 
    {
        NOTEPITCH_NONOTE	=	-1,
        NOTEPITCH_MIN		=	0,
        NOTEPITCH_OCTAVE	=	12,
        NOTEPITCH_MAX		=	(NOTEPITCH_OCTAVE*7)
    };
	
    // note:
    enum SymphonieChunkType
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
        CT_DELTASAMPLE = 0xFFFFFFEF,    // -17 8-bit deltapacked sample
        CT_DELTA16 = 0xFFFFFFEE,        // -18 16-bit deltapacked sample
        CT_INFOTYPE = 0xFFFFFFED,       // -19
        CT_INFOOBJ = 0xFFFFFFEC,        // -20
        CT_STRING = 0xFFFFFFEB,         // -21
        
        CT_NGSAMPLEBOOST = 0x0000000A,  // 10
        CT_PITCHDIFF = 0x0000000B,      // 11
        CT_SAMPLEDIFF = 0x0000000C,     // 12
        
        CT_EOF = 0x00000000             // end of file
    };

    class SyMMInstrument
    {
    public:
        SyMMInstrument()
            : m_name()
            , m_number(0)
            //, m_data()
            , m_bIsVirtual(false)
            , m_type(0)
			//, m_sampleFormat()
        {}
        ~SyMMInstrument()
        {
            //delete m_data.m_pBuf;
        }
        std::string m_name;
        size_t m_number; // index-value of instrument (needed somewhere..?)
        //bufferedData_t m_data;
        bool m_bIsVirtual;
        size_t m_type;
		
		// TODO: unpack to simple common type instead?
        //SyMMSampleInfo_t m_sampleFormat;
		CAudioSample *m_pSample;
    };
	size_t m_currentInstrument; // instrument index until all found..?
    SyMMInstrument *m_pInstruments;
    //size_t m_instrumentCount_; // CHECK! already in CT_SAMPLENUMB ??
    
    class SyMMSequence
    {
    public:
        SyMMSequence()
            : m_sequenceCount(0)
            , m_startPosition(0)
            , m_endPosition(0)
            , m_action(0)
            , m_tune(0)
            , m_loopCount(0)
        {}
        ~SyMMSequence()
        {}
        size_t m_sequenceCount;
        size_t m_startPosition;
        size_t m_endPosition;
        size_t m_action;
        size_t m_tune;
        size_t m_loopCount;
    };
    SyMMSequence *m_pSequences;
    size_t m_sequenceCount;

    
    class SyMMPosition
    {
    public:
        SyMMPosition()
            : m_layerCount(0)
            , m_tune(0)
            , m_startRow(0)
            , m_rowLength(0)
            , m_loopCount(0)
            , m_speedCycl(0)
            //, m_PatternNumbers()
            , m_nPatternNumber(0)
        {}
        ~SyMMPosition()
        {
            // does this have to be a buffer
            // when single uint16 seems to be enough?
            //delete m_PatternNumbers.m_pBuf;
        }
        size_t m_layerCount;
        size_t m_tune;
        size_t m_startRow;
        size_t m_rowLength;
        size_t m_loopCount;
        size_t m_speedCycl;
        
        // TODO: ?
        // does this have to be a buffer
        // when single uint16 seems to be enough?
        //bufferedData_t m_PatternNumbers;
        size_t m_nPatternNumber;
    };
    SyMMPosition *m_pPositions;
    size_t m_positionCount;
    
    // CT_NOTEDATA
    bufferedData_t m_PatternData;
    size_t m_nPatternCount;
    size_t m_nPatternSize;
    
    // CT_INFOTEXT
    std::string m_comments;
    
    // values of single-value parameters
    int32_t m_audioChannelcount;    // CT_CHANNELNUMB
    int32_t m_trackLength;          // CT_TRACKLEN
    int32_t m_totalPatternlength;   // CT_PATTERNNUMB
    int32_t m_instrumentCount;      // CT_SAMPLENUMB
    int32_t m_songEventSize;        // CT_NOTESIZE
    int32_t m_systemBPM;            // CT_SYSTEMSPEED
    int32_t m_isPureSong;           // CT_ISSONG
    
    int32_t m_globalSampleBoost;    // CT_NGSAMPLEBOOST
    int32_t m_stereoEncPitchDiff;   // CT_PITCHDIFF
    int32_t m_stereoEncSampleDiff;  // CT_SAMPLEDIFF

    // always 1..?
    long m_lVersion;
    
	bool UnpackRunlen(const uint8_t *pOrigData, const size_t nLen, uint8_t *pOutBuf, size_t nUnpackLen);
    // TODO:..
    bool Decode8bitSample(const uint8_t *pData, const size_t nLen);
    bool Decode16bitSample(const uint8_t *pData, const size_t nLen);
	
	//SyMMSampleFormat DetermineSampleFormat(uint8_t *pData, const size_t nLen);
	CAudioSample *HandleSampleFormat(uint8_t *pData, const size_t nLen);
    
    bool OnChunk(uint32_t chunkID);
    bool OnLargeChunk(uint32_t chunkID);
    
    bool OnPatternData(uint8_t *pData, const size_t nLen);
    bool OnInstrumentNames(uint8_t *pData, const size_t nLen);

    bool OnInstrumentSample(uint8_t *pData, const size_t nLen);
    bool OnSequence(uint8_t *pData, const size_t nLen);
    bool OnSongPositions(uint8_t *pData, const size_t nLen);
    
public:
    CSymphoniePlayer(CReadBuffer *pFileData);
    virtual ~CSymphoniePlayer();
    
    virtual bool ParseFileInfo();
    
    // TODO:
    virtual size_t DecodePlay(void *pBuffer, const size_t nBufSize);
};

#endif // SYMPHONIEPLAYER_H
