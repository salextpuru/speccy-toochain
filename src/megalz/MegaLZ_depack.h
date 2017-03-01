#ifndef MEGALZ_DEPACK_H
#define MEGALZ_DEPACK_H

#include "MegaLZ_types.h"


// getting byte for use of depacker();
ULONG get_byte(void);

// putting outbut buffer to file: for use of depacker();
ULONG put_buffer(ULONG pos,ULONG size);

// main depacking function
ULONG depack(void);



#endif

