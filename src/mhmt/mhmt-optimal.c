#include <stdio.h>
#include <stdlib.h>
#include "mhmt-types.h"
#include "mhmt-optimal.h"
// allocate place for optimal chain building amd initialize it
struct optchain * make_optch(ULONG actual_len)
{
    struct optchain * optch;
    ULONG i;
    // we allocate length+1 because all codes at the end of input stream will point
    // to the length+1 place. Also we'll start reversing from length+1 position in optch array
    optch = (struct optchain *)malloc( (actual_len+1)*sizeof(struct optchain) );

    if( optch ) {
        optch[0].code.length = 1; // 1st byte is always copied 'as-is', however, this is just filler,
        optch[0].code.disp   = 0; // not accounted elsewhere
        // init prices to absolute maximum for optimal chain build-up
        optch[0].price = 0;
        optch[1].price = 8;

        for(i=2; i<(actual_len+1); i++)
            optch[i].price = 0xFFFFFFFF;
    }

    return optch;
}
// free optchain array
void free_optch(struct optchain * optch)
{
    if( optch )
        free( optch );
}
// update prices at the position given all lzcodes.
// it also needs pointer to the function that calculates bit length of given LZ code
void update_optch(ULONG position, struct lzcode * codes, ULONG (*get_lz_price)(ULONG position, struct lzcode * lzcode), struct optchain * optch)
{
    ULONG codepos;
    ULONG bitlen;
    ULONG newpos;
    LONG len;

    for( codepos = 0; len=codes[codepos].length; codepos++ ) { // loop through all existing lz codes
        bitlen = (*get_lz_price)(position, &codes[codepos]); // get bit length of given lz code

        if( !bitlen ) {
            printf("mhmt-optimal.c: update_optch() found zero bitlength of lz code. Fatal error.\n");
            exit(1);

        } else {
            if( len<0 ) len=(-len); // deal with negative lengths (special markers)

            newpos = position + len; // look where current lz code points to and take from there old price reaching that location

            if( optch[newpos].price > bitlen + optch[position].price ) { // if oldprice is worse than with current lz code
                optch[newpos].price = bitlen + optch[position].price;
                optch[newpos].code  = codes[codepos];
            }
        }
    }
}
// reverses optimal chain making it ready for scanning (fetching optimal chain)
//
void reverse_optch(struct optchain * optch, ULONG actual_len)
{
    struct lzcode curr, temp;
    ULONG position;
    LONG len;
    position = actual_len;
    temp = optch[position].code;

    while(position>1) {
        len = temp.length;

        if( len<0 ) len=(-len);

        position -= len;
        curr = temp;
        temp = optch[position].code;
        optch[position].code = curr;
    }
}
