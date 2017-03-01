#include <stdio.h>
#include <stdlib.h>
#include "mhmt-types.h"
#include "mhmt-hash.h"
// allocate mem for hash (length) and build it from data
// length must be at least 3 bytes, since hash[0] and hash[1] are not valid hashes
UBYTE * build_hash(UBYTE * data, ULONG length)
{
    UBYTE * hash;
    ULONG i;
    UBYTE *src,*dst;
    UBYTE curr,prev,prev2;

    if( !length )
        return NULL;

    hash=(UBYTE *)malloc( length );

    if( !hash )
        return NULL;

    prev=curr=0;
    i=length;
    src = data;
    dst = hash;

    do {
        prev2 = (UBYTE)( (prev>>1) | (prev<<7) );
        prev  = (UBYTE)( (curr>>1) | (curr<<7) );
        curr  = *(src++);
        *(dst++) = curr^prev^prev2;
    } while( --i );

    return hash;
}
// free hash
void destroy_hash(UBYTE * hash)
{
    if( hash ) free(hash);
}
