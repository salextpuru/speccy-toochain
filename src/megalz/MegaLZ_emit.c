#include <stdio.h>
#include <string.h>

#include "MegaLZ_emit.h"

#include "MegaLZ_globals.h"



// buffer for outting output stream to file
UBYTE oubuf[OUBUFSIZE];


// position of byte in output buffer where bits are written
ULONG ob_bitpos;

// current free position in output buffer
ULONG ob_freepos;

// num of bits in 'bit'-byte (pointed to by ob_bitpos)
ULONG ob_bits;





ULONG emit_init(void)
{ // inits output stream emitter

#ifdef DEBUG
	ULONG i;

	for(i=0;i<OUBUFSIZE;i++)
		oubuf[i]=0;
#endif

	oubuf[0]=indata[0]; // copy first byte 'as-is'
	ob_bitpos=1;
	ob_freepos=2;
	ob_bits=0;

	return 1;
}


ULONG emit_code(struct lzinfo * lz)
{ // emits given lz code to the output stream

	// write bits

	emit_bits(lz->bits,lz->bitsnum);


	// write byte, if any

	if( lz->byte!=0xFFFFFFFF )
	{
		oubuf[ob_freepos++]=(UBYTE)(lz->byte&0x000000FF);
	}


	// flush to disk if needed

	if( ob_freepos>(OUBUFSIZE-8) )
	{
		// write up to current bit position
		if( ob_bitpos!=fwrite( oubuf, 1, ob_bitpos, file_out ) )  return 0;

		// move remaining part of buffer to the beginning
		memcpy( oubuf, &oubuf[ob_bitpos], OUBUFSIZE-ob_bitpos);

		// update pointers
		ob_freepos-=ob_bitpos;
		ob_bitpos=0;
	}

	return 1;
}

ULONG emit_finish(void)
{ // finishes output stream outting (flushes everyth, writes stopping bitcode)

	// write stopping bitcode
	emit_bits(0x60100000,12);

	// fill up last unfinished 'bit'-byte
	while( ob_bits<8 )
	{
		oubuf[ob_bitpos]<<=1;
		ob_bits++;
	}

	// write remaining part of buffer
	if( ob_freepos!=fwrite( oubuf, 1, ob_freepos, file_out ) )  return 0;

	return 1;
}


void emit_bits(ULONG bits,ULONG bitsnum)
{
	ULONG i,shifter;

	shifter=bits;

	for(i=0;i<bitsnum;i++)
	{
		if( ob_bits==8 )
		{
			ob_bitpos=ob_freepos;
			ob_freepos++;
			ob_bits=0;
		}

		oubuf[ob_bitpos]=(UBYTE)( (oubuf[ob_bitpos]<<1) | (UBYTE)( shifter>>31 ));

		ob_bits++;

		shifter<<=1;
	}
}

