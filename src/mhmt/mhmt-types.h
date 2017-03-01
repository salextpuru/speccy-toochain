#ifndef MHMT_TYPES_H
#define MHMT_TYPES_H
#ifdef _AMIGA
#include <exec/types.h>
#else
#define MHMT_OWNTYPES
#endif
#ifdef MHMT_OWNTYPES
typedef        signed char  BYTE;
typedef      unsigned char UBYTE;
typedef   signed short int  WORD;
typedef unsigned short int UWORD;
typedef         signed int  LONG;
typedef       unsigned int ULONG;
#endif
#endif
