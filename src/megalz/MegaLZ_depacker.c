#include "MegaLZ_depacker.h"

// prototypes of get_byte() and put_buffer()
#include "MegaLZ_depack.h"



UBYTE dbuf[DBSIZE];
ULONG dbpos; // current position in buffer (wrappable)


UBYTE bitstream;
ULONG bitcount;

#define ERRORS
// for complete error checking in depacker()
// remove if you don't need any error checking


ULONG depacker(void)
{
	ULONG i;

	ULONG dbflush; // position from which to flush buffer

	ULONG byte;
	ULONG bits;

	LONG disp;

	ULONG finished;

	// init depack buffer with zeros
	for(i=0;i<DBSIZE;i++) dbuf[i]=0;


	dbpos=0;
	dbflush=0;

	// get first byte of packed file and write to output
	byte=get_byte();
#ifdef ERRORS
	if( byte==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
	dbuf[dbpos++]=(UBYTE)byte;


	// second byte goes to bitstream
	byte=get_byte();
#ifdef ERRORS
	if( byte==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
	bitstream=(UBYTE)byte;
	bitcount=8;


	// actual depacking loop!
	finished=0;
	do
	{
		// get 1st bit - either OUTBYTE (see MegaLZ_lz.h) or beginning of LZ code
		bits=get_bits(1);
#ifdef ERRORS
		if( bits==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif

		if( bits )
		{ // OUTBYTE
                        byte=get_byte();
#ifdef ERRORS
			if( byte==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
			dbuf[(dbpos++)&DBMASK]=(UBYTE)byte;
		}
		else
		{ // LZ code
			bits=get_bits(2);
#ifdef ERRORS
			if( bits==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif

			switch( bits )
			{
			case 0: // 000
				bits=get_bits(3);
#ifdef ERRORS
				if( bits==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
				repeat((LONG)(0xFFFFFFF8|bits),1);
				break;
			case 1: // 001
				byte=get_byte();
#ifdef ERRORS
				if( byte==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
				repeat((LONG)(0xFFFFFF00|byte),2);
				break;
			case 2: // 010
				disp=get_bigdisp();
#ifdef ERRORS
				if( !disp ) return DEPACKER_NOEOF;
#endif
				repeat(disp,3);
				break;
			case 3: // 011
				// extract num of length bits
				i=0;
				do
				{
					bits=get_bits(1);
#ifdef ERRORS
					if( bits==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
					i++;
				} while( !bits );

				// check for exit code
				if( i==9 )
				{
					finished=1;
				}
				else if( i<=7 )
				{
					// get length bits itself
					bits=get_bits(i);
#ifdef ERRORS
					if( bits==0xFFFFFFFF ) return DEPACKER_NOEOF;
#endif
                        	        disp=get_bigdisp();
#ifdef ERRORS
					if( !disp ) return DEPACKER_NOEOF;
#endif
					repeat(disp,2+(1<<i)+bits);
				}
#ifdef ERRORS
				else
				{
					return DEPACKER_ERRCODE;
				}
#endif
				break;
#ifdef ERRORS
			default:
				return DEPACKER_ERRCODE; // although this should NEVER happen!
#endif
			}
		}

		// check if we need flushing buffer
		if( (((dbpos-dbflush)&DBMASK)>(DBSIZE-257)) || finished )
		{
#ifdef ERRORS
			if( !put_buffer(dbflush&DBMASK,(dbpos-dbflush)&DBMASK) )
			{
				return DEPACKER_CANTWRITE;
			}
#else
			put_buffer(dbflush&DBMASK,(dbpos-dbflush)&DBMASK);
#endif
			dbflush=dbpos;
		}

	} while( !finished );


#ifdef ERRORS
	return (get_byte()==0xFFFFFFFF)?DEPACKER_OK:DEPACKER_TOOLONG;
#else
	return DEPACKER_OK:
#endif
}



ULONG get_bits(ULONG numbits)
{ // gets specified number of bits from bitstream
  // returns them LSB-aligned
  // if error (get_byte() fails) return 0xFFFFFFFF, so function can't get more than 31 bits!
  // if 0 bits required, returns 0

	ULONG count;
	ULONG bits;
	ULONG input;

	bits=0;
	count=numbits;

	while(count--)
	{
		if( bitcount-- )
		{
			bits<<=1;
			bits|=0x00000001&(bitstream>>7);
			bitstream<<=1;
		}
		else
		{
			bitcount=8;
			input=get_byte();
#ifdef ERRORS
			if( input==0xFFFFFFFF ) return 0xFFFFFFFF;
#endif
			bitstream=(UBYTE)input;

			count++;    // repeat loop once more
		}
	}

	return bits;
}

void repeat(LONG disp,ULONG len)
{ // repeat len bytes with disp displacement (negative)
  // uses dbpos & dbuf

	ULONG i;

	for(i=0;i<len;i++)
	{
		dbuf[DBMASK&dbpos]=dbuf[DBMASK&(dbpos+disp)];
		dbpos++;
	}
}

LONG get_bigdisp(void)
{ // fetches 'big' displacement (-1..-4352)
  // returns negative displacement or ZERO if failed either get_bits() or get_byte()

	ULONG bits,byte;

	bits=get_bits(1);
#ifdef ERRORS
	if( bits==0xFFFFFFFF ) return 0;
#endif

	if( bits )
	{ // longer displacement
		bits=get_bits(4);
#ifdef ERRORS
		if( bits==0xFFFFFFFF ) return 0;
#endif
		byte=get_byte();
#ifdef ERRORS
		if( byte==0xFFFFFFFF ) return 0;
#endif
		return (LONG)( ((0xFFFFF000|(bits<<8))-0x0100)|byte );
	}
	else
	{ // shorter displacement
		byte=get_byte();
#ifdef ERRORS
		if( byte==0xFFFFFFFF ) return 0;
#endif
		return (LONG)( 0xFFFFFF00|byte );
	}
}

