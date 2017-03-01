#include <stdlib.h>
#include "mhmt-globals.h"
struct globals wrk;
// init wrk container
void init_globals(void)
{
    wrk.packtype = PK_MLZ;
    wrk.greedy   = 0;
    wrk.mode     = 0;
    wrk.zxheader = 0;
    wrk.wordbit  = 0;
    wrk.bigend   = 0;
    wrk.fullbits = 0;
    wrk.maxwin   = 4352;
    wrk.fname_in  = NULL;
    wrk.fname_out = NULL;
    wrk.file_in  = NULL;
    wrk.file_out = NULL;
    wrk.indata = NULL;
    wrk.inlen = 0;
}
// free all stuff from wrk container
void free_globals(void)
{
    if( wrk.indata ) free( wrk.indata );

    if( wrk.file_out ) fclose( wrk.file_out );

    if( wrk.file_in )  fclose( wrk.file_in );

    if( wrk.fname_out ) free( wrk.fname_out );

    if( wrk.fname_in )  free( wrk.fname_in );
}
