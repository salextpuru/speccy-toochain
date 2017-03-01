#ifndef MHMT_HASH_H
#define MHMT_HASH_H
#include "mhmt-types.h"
UBYTE * build_hash(UBYTE * data, ULONG length);
void destroy_hash(UBYTE * hash);
#endif
