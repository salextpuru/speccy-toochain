#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MegaLZ_lz.h"



struct lzcode codes[256]; // for the current byte contain all possible codes from .length=1 to .length=255,
                          // stopflag - .length=1, .disp=0 (also means OUTBYTE)
ULONG codepos; // current position in codes[] array




void start_lz(void)
{ // re-init codes[] array for next byte
	codepos=0;
}

void end_lz(void)
{ // ends codes[] array filling by setting stopflag
	codes[codepos].disp=0;
	codes[codepos].length=1;

 if(codepos>=256) printf("end_lz(): overflow!\n");
}


void add_lz(LONG disp, ULONG length)
{ // adds LZ code to the codes[] array
  // position - from which point LZ code starts in input file, (position-curpos) - back displacement

	codes[codepos].length = (UWORD)length;
	codes[codepos].disp   = (WORD)disp;
	codepos++;
}


ULONG make_lz_info(UBYTE curbyte,struct lzcode * lzcode,struct lzinfo * lzinfo)
{ // on the given lzcode element, returns total bit length of this code,
  // if given ptr to lzinfo, writes there precise info over lz code
  // if lzcode==NULL or lzcode->length==0 - treated as OUTBYTE type code

  // returns 0 for wrong lz length/displacement combination, does not fill lzinfo

  // curbyte needed only for OUTBYTE lz code, can be given any value otherwise

	ULONG type,length,add_length,len,bits;
	ULONG ptrn;


	// first determine bit length of resulting LZ code
	length=0;
	if( lzcode==NULL )
	{
		length=9; // OUTBYTE
		type=OUTBYTE;
	}
	else if( lzcode->disp==0 )
	{
		length=9;
		type=OUTBYTE;
	}
	else
	{
		if( lzcode->length==1 )
		{
			if( (lzcode->disp>=(-8))&&(lzcode->disp<=(-1)) )
			{
				length=6; // LEN1
				type=LEN1;
			}
		}
		else if( lzcode->length==2 )
		{
			if( (lzcode->disp>=(-256))&&(lzcode->disp<=(-1)) )
			{
				length=11; // LEN2
				type=LEN2;
			}
		}
		else if( lzcode->length==3 )
		{
			if( (lzcode->disp>=(-256))&&(lzcode->disp<=(-1)) )
			{
				length=12; // LEN3_SHORT
				type=LEN3_SHORT;
			}
			else if( (lzcode->disp>=(-4352))&&(lzcode->disp<=(-257)) )
			{
				length=16; // LEN3_LONG
				type=LEN3_LONG;
			}
		}
		else if( lzcode->length<=255 )
		{
			if( (lzcode->disp>=(-4352))&&(lzcode->disp<=(-1)) )
			{
				if( (lzcode->disp>=(-256)) )
				{
					add_length=12; // VARLEN_SHORT
					type=VARLEN_SHORT;
				}
				else
				{
					add_length=16; // VARLEN_LONG
					type=VARLEN_LONG;
				}

				// calc bits for size
				len=(lzcode->length - 2)>>1;

				bits=0;
				while( len )
				{
					bits++;
					len>>=1;
				}

				length=bits+bits+add_length; // final length
			}
		}
	}

	// if length==0, error in code!
	if( length==0 ) return 0; // error flag




	// see if we need to fill struct lzinfo
	if( lzinfo )
	{

		// fill already known values
		lzinfo->type=type;

		if( lzcode )
		{
			lzinfo->disp=lzcode->disp;
			lzinfo->length=lzcode->length;
		}
		else
		{
			lzinfo->disp=0; // only for OUTBYTE
			lzinfo->length=1;
		}

		lzinfo->bitsize=length;

		// fill remaining values
		switch(type)
		{
		case OUTBYTE:
			lzinfo->bits=0x80000000;
			lzinfo->bitsnum=1;       // one '1' bit

			lzinfo->byte=curbyte;
		break;

		case LEN1:
			lzinfo->bits=((((ULONG)lzinfo->disp)&7)<<26);
			lzinfo->bitsnum=6;

			lzinfo->byte=0xFFFFFFFF; // no byte
		break;

		case LEN2:
			lzinfo->bits=0x20000000;
			lzinfo->bitsnum=3;

			lzinfo->byte=((ULONG)lzinfo->disp)&0x00FF;
		break;

		case LEN3_SHORT:
			lzinfo->bits=0x40000000;
			lzinfo->bitsnum=4;

			lzinfo->byte=((ULONG)lzinfo->disp)&0x00FF;
		break;

		case LEN3_LONG:
			lzinfo->bits=0x50000000|(((((ULONG)lzinfo->disp)+0x0100)&0x00000F00)<<16);
			lzinfo->bitsnum=8;

			lzinfo->byte=((ULONG)lzinfo->disp)&0x00FF;
		break;

		case VARLEN_SHORT:
			ptrn=0xFFFFFFFF>>(32-bits);
			ptrn=(ptrn&( lzinfo->length - (1<<bits) - 2 ));
			ptrn=ptrn|(1<<bits);
			ptrn<<=1;
			ptrn=0x60000000|(ptrn<<(28-bits-bits));

			lzinfo->bits=ptrn;
			lzinfo->bitsnum=length-8;

			lzinfo->byte=((ULONG)lzinfo->disp)&0x00FF;
		break;

		case VARLEN_LONG:
			ptrn=0xFFFFFFFF>>(32-bits);
			ptrn=(ptrn&( lzinfo->length - (1<<bits) - 2 ));
			ptrn=ptrn|(1<<bits);
			ptrn=(ptrn<<1)|1;
			ptrn=(ptrn<<4)|(((((ULONG)lzinfo->disp)+0x0100)&0x00000F00)>>8);
			ptrn=0x60000000|(ptrn<<(24-bits-bits));

			lzinfo->bits=ptrn;
			lzinfo->bitsnum=length-8;

			lzinfo->byte=((ULONG)lzinfo->disp)&0x00FF;
		break;
		}
	}

	return length;
}

