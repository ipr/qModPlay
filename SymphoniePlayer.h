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
public:
    CSymphoniePlayer(const CReadBuffer *pFileData);
};

#endif // SYMPHONIEPLAYER_H
