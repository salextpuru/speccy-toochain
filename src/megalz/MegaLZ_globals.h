#ifndef MEGALZ_GLOBALS_H

#define MEGALZ_GLOBALS_H


#include <stdio.h>
#include "MegaLZ_types.h"


extern char * fname_in;  //filename of input file
extern char * fname_out; // --//-- of output file

extern FILE * file_in;  //file descr of input file
extern FILE * file_out; //file descr of output file

extern ULONG inlen; //length of input file

extern UBYTE * indata; // input data, loaded into mem

extern ULONG mode; // mode of working:
#define MODE_PACK_OPTIMAL 1
#define MODE_PACK_GREEDY  2
#define MODE_DEPACK       3


#endif

