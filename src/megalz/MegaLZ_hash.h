#ifndef MEGALZ_HASH_H
#define MEGALZ_HASH_H

#include "MegaLZ_types.h"
#include "MegaLZ_globals.h"

extern UBYTE * hash;


ULONG init_hash(void);
void free_hash(void);
void make_hash(void);


#endif

