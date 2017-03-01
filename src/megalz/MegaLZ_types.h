#ifndef MEGALZ_TYPES_H

#define MEGALZ_TYPES_H


//#ifdef __arm
//#define MEGALZ_OWNTYPES
//#endif
//#ifdef _WIN32
//#define MEGALZ_OWNTYPES
//#endif

#ifdef _AMIGA
#include <exec/types.h>
#else
#define MEGALZ_OWNTYPES
#endif

#ifdef MEGALZ_OWNTYPES
typedef        signed char  BYTE;
typedef      unsigned char UBYTE;

typedef   signed short int  WORD;
typedef unsigned short int UWORD;

typedef         signed int  LONG;
typedef       unsigned int ULONG;
#endif

#endif

