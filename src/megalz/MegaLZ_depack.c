#include <stdio.h>

#include "MegaLZ_depack.h"
#include "MegaLZ_depacker.h"
#include "MegaLZ_globals.h"

ULONG inpos;

ULONG depack(void)
{ // main depacking function

	// some inits
	inpos=0;

	return (depacker()==DEPACKER_OK)?1:0;
}



// callbacks for depacker() below

ULONG get_byte(void)
{ // gets byte from input stream
	
	return (inpos<inlen)?(ULONG)indata[inpos++]:0xFFFFFFFF;
}

ULONG put_buffer(ULONG pos,ULONG size)
{ // writes specified part of buffer to the output file (with wrapping!)
	ULONG size1,size2,success;

	if( (pos+size)<=DBSIZE )
	{ // no wrapping needed
		return (size==fwrite(&dbuf[pos],1,size,file_out))?1:0;
	}
	else
	{ // wrapping
		size1=(DBSIZE-pos);
		size2=size-size1;

		success=  (size1==fwrite(&dbuf[pos],1,size1,file_out))?1:0;
		success&= (size2==fwrite(&dbuf[0],  1,size2,file_out))?1:0;
		return success;
	}
}

