#ifndef MEGALZ_DEPACKER_H
#define MEGALZ_DEPACKER_H

#include "MegaLZ_types.h"

// size and mask of output buffer
#define DBSIZE 8192
#define DBMASK 8191

extern UBYTE dbuf[DBSIZE];


// depacker return codes
//
// all OK
#define DEPACKER_OK        1
// erroneous LZ code encountered
#define DEPACKER_ERRCODE   2
// unexpected end of input stream without STOP code
#define DEPACKER_NOEOF     3
// after STOP code there are more bytes in input stream
#define DEPACKER_TOOLONG   4
// to indicate put_buffer() fail
#define DEPACKER_CANTWRITE 5

ULONG depacker(void);

ULONG get_bits(ULONG);

void repeat(LONG,ULONG);

LONG get_bigdisp(void);




#endif

