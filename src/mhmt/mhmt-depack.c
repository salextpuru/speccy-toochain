#include <stdio.h>
#include <stdlib.h>
#include "mhmt-types.h"
#include "mhmt-globals.h"
#include "mhmt-depack.h"
#include "mhmt-lz.h"
#include "mhmt-emit.h"
ULONG buf_size=0;
ULONG buf_ptr=0;
UBYTE * buffer=NULL;
ULONG depack(void)
{
    ULONG (*checker) (void) = NULL;
    ULONG (*depacker)(void) = NULL;
    ULONG success=1;

    // some preparations
    //
    if( wrk.packtype==PK_MLZ ) {
        checker  = &checker_megalz;
        depacker = &depacker_megalz;

    } else if( wrk.packtype==PK_HRM ) {
        checker  = &checker_hrum;
        depacker = &depacker_hrum;

    } else if( wrk.packtype==PK_HST ) {
//		checker  = &checker_hrust;
        depacker = &depacker_hrust;

    } else {
        printf("mhmt-depack.c:depack() - format unsupported!\n");
        return 0;
    }

    // allocate buffer used for depacking
    //
    buf_size = ( wrk.maxwin==4352 ) ? 8192 : wrk.maxwin; // provided there are no other non-2^n sizes
    buffer=(UBYTE*)malloc(buf_size);

    if( !buffer ) {
        printf("mhmt-depack.c:depack() cannot allocate memory for depack buffer!\n");
        return 0;
    }

    buf_ptr=0;
    success = success && emit_file(NULL,EMIT_FILE_INIT);
//	success = success && (*checker) ();
//#ifdef DBG
//	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
//#endif
    success = success && (*depacker)();
    /*success = success && */depack_outbyte( 0, DEPACK_OUTBYTE_FLUSH );
    /*success = success && */emit_file(NULL,EMIT_FILE_FINISH);

    if( buffer )
        free(buffer);

    return success;
}
// checks input file for consistency
#undef  DPK_DEPACK
#define DPK_CHECK
#define DPK_REPERR
ULONG checker_megalz(void)
#include "mhmt-depack-megalz.c"
ULONG checker_hrum(void)
#include "mhmt-depack-hrum.c"
//ULONG checker_hrust(void)
//#include "mhmt-depack-hrust.c"
//
// actually depacks without checkings
#define DPK_DEPACK
#undef  DPK_CHECK
#undef  DPK_REPERR
ULONG depacker_megalz(void)
#include "mhmt-depack-megalz.c"
ULONG depacker_hrum(void)
#include "mhmt-depack-hrum.c"
#define  DPK_CHECK
#define  DPK_REPERR
ULONG depacker_hrust(void)
#include "mhmt-depack-hrust.c"
// rewind - to the beginning of input stream, byte - next byte
// returns 0xFFFFFFFF if error (exhausted stream), otherwise byte (0..255)
ULONG depack_getbyte(ULONG operation)
{
    static ULONG position;

    if( operation==DEPACK_GETBYTE_REWIND ) {
        position=0;
        return 0;

    } else if( operation==DEPACK_GETBYTE_NEXT ) {
        if( position < wrk.inlen ) {
//#ifdef DBG
//			printf("<%02x>", wrk.indata[position]);
//#endif
            return (ULONG)wrk.indata[position++];
        }

    } else // should never happen in a correct program
        printf("mhmt-depack.c:depack_get() - wrong operation code\n");

    return 0xFFFFFFFF;
}
//#define DEPACK_GETBITS_FORCE 1
//#define DEPACK_GETBITS_NEXT  2
//
// returns 0xFFFFFFFF if error, otherwise LSB-aligned, zero-extended bits
ULONG depack_getbits(ULONG numbits, ULONG operation)
{
    static ULONG bits;
    static ULONG num_bits_left;
    ULONG fetched_bits;

    if( operation==DEPACK_GETBITS_FORCE ) { // force word retrieval (for start of stream)
        bits = depack_getbits_word();

        if( bits==0xFFFFFFFF) return 0xFFFFFFFF;

        num_bits_left = wrk.wordbit ? 16 : 8;
        return 0;

    } else if( operation==DEPACK_GETBITS_NEXT ) { // return bits and fetch new as needed (wrk.fullbits accounted for)
        if( (numbits==0) || (numbits>31) ) {
            printf("mhmt-depack.c:depack_getbits() - too many (>31) or zero bits requested\n");
            return 0xFFFFFFFF;
        }

        fetched_bits = 0;

        do {
            if( !wrk.fullbits ) { // empty bits
                if( !num_bits_left ) {
                    bits = depack_getbits_word();

                    if( bits==0xFFFFFFFF) return 0xFFFFFFFF;

                    num_bits_left = wrk.wordbit ? 16 : 8;
                }
            }

//#ifdef DBG
//			printf("%d",bits>>31);
//#endif
            fetched_bits = ( fetched_bits<<1 ) | ( 1&(bits>>31) );
            bits <<= 1;
            num_bits_left--;

            if( wrk.fullbits ) {
                if( !num_bits_left ) {
                    bits = depack_getbits_word();

                    if( bits==0xFFFFFFFF) return 0xFFFFFFFF;

                    num_bits_left = wrk.wordbit ? 16 : 8;
                }
            }
        } while( --numbits );

        return fetched_bits;

    } else {
        printf("mhmt-depack.c:depack_getbits() - wrong operation code\n");
        return 0xFFFFFFFF;
    }
}
// gets word of bits (UBYTE or UWORD), accounts for big-little endian
// returns 0xFFFFFFFF if no bytes in input stream (depack_getbyte()), otherwise
// left-aligned bits.
ULONG depack_getbits_word(void)
{
    ULONG bits,bits2;

    if( wrk.wordbit ) { // 16bits
        if( wrk.bigend ) {
            bits  = depack_getbyte(DEPACK_GETBYTE_NEXT);

            if( bits  == 0xFFFFFFFF ) return 0xFFFFFFFF;

            bits2 = depack_getbyte(DEPACK_GETBYTE_NEXT);

            if( bits2 == 0xFFFFFFFF ) return 0xFFFFFFFF;

        } else {
            bits2 = depack_getbyte(DEPACK_GETBYTE_NEXT);

            if( bits2 == 0xFFFFFFFF ) return 0xFFFFFFFF;

            bits  = depack_getbyte(DEPACK_GETBYTE_NEXT);

            if( bits  == 0xFFFFFFFF ) return 0xFFFFFFFF;
        }

        bits = (bits<<24) | ( 0x00FF0000&(bits2<<16) );

    } else { // 8bits
        bits=depack_getbyte(DEPACK_GETBYTE_NEXT);

        if( bits!=0xFFFFFFFF)
            bits <<= 24;
    }

    return bits;
}
// puts byte to the output buffer. if it is full, flushes via mhmt-emit.c:emit_file()
// relies on initialized globals: buffer, buf_size, buf_ptr
// returns zero if error (in emit_file()), otherwise non-zero
ULONG depack_outbyte(UBYTE byte, ULONG operation)
{
    if( operation==DEPACK_OUTBYTE_ADD ) {
        buffer[buf_ptr++] = byte;

        if( buf_ptr>0x51d ) {
//			printf("слом\n");
        }

        if( buf_ptr >= buf_size ) {
            buf_ptr=0;
            return emit_file( buffer, buf_size );
        }

        return 1;

    } else if( operation==DEPACK_OUTBYTE_FLUSH ) {
        if( buf_ptr ) return emit_file( buffer, buf_ptr );

        return 1;

    } else {
        printf("mhmt-depack.c:depack_outbyte() - bad operation requested\n");
        return 0;
    }
}
// repeats data in output buffer, flushes buffer if needed.
// relies on initialized globals, also relies on buf_size being 2^N
// displacement is back-displacement (negative)
// non-zero if success
ULONG depack_repeat(LONG disp, ULONG length)
{
    ULONG back_ptr;
    ULONG success=1;

//#ifdef DBG
//	printf("\n");
//#endif
    // in a self-consistent system, these three errors should never appear, since there is input stream check before actual depacking
    if( !length ) {
        printf("mhmt-depack.c:depack_repeat() - zero length!\n");
        return 0;

    } else if( disp>=0 ) {
        printf("mhmt-depack.c:depack_repeat() - non-negative displacement!\n");
        return 0;

    } else if( (ULONG)(-disp)>buf_size ) {
        printf("mhmt-depack.c:depack_repeat() - displacement greater than buffer size!\n");
        return 0;

    } else {
        back_ptr = (disp+buf_ptr) & (buf_size-1); // buf_size MUST BE 2^N!

        do {
            success = success && depack_outbyte( buffer[back_ptr], DEPACK_OUTBYTE_ADD ); // also increases buf_ptr
            back_ptr = (back_ptr+1) & (buf_size-1); // buf_size MUST BE 2^N!
        } while( --length );
    }

    return success;
}
