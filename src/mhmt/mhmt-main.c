#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhmt-types.h"
#include "mhmt-globals.h"
#include "mhmt-parsearg.h"
#include "mhmt-pack.h"
#include "mhmt-depack.h"
void show_help(void);
void dump_config(void);
ULONG do_files(void);
int main( int argc, char* argv[] )
{
    int error=0;
    ULONG parse_result;
    init_globals();
    // printf short info
    printf("mhmt - MeHruMsT - MEgalz, HRUM and hruST (c) 2009 lvd^nedopc\n\n");
    // parse arguments
    parse_result = parse_args(argc, argv);

    if( parse_result&ARG_PARSER_SHOWHELP ) {
        if( parse_result&ARG_PARSER_ERROR )
            printf("\n");

        show_help();
    }

    if( parse_result&ARG_PARSER_ERROR ) {
        printf("There were errors in arguments.\n");
        error++;

    } else if( parse_result&ARG_PARSER_GO ) {
        if( do_files() ) {
            dump_config();

            if( wrk.mode ) {
                error += depack() ? 0 : 1;

            } else {
                error += pack() ? 0 : 1;
            }

        } else {
            error++;
        }
    }

    free_globals();
    return error;
}
void show_help(void)
{
    printf("======== mhmt help ========\n");
    printf("parameters:\n");
    printf("-mlz, -hrm, -hst - use MegaLZ, hrum3.5 or hrust1.x format (default is MegaLZ)\n");
    printf("-g - greedy coding (default is optimal coding)\n");
    printf("-d - depacking instead of packing (default is packing)\n");
    printf("\n");
    printf("-zxh - use zx-specific header for hrum or hrust. DEFAULT is NO HEADER!\n");
    printf("       Not applicable for MegaLZ. If -zxh is specified, -16, NO -bend and\n");
    printf("       NO -mlz is forced.\n");
    printf("\n");
    printf("-8, -16 - bitstream is in bytes or words in packed file.\n");
    printf("          Default for MegaLZ is -8, for hrum and hrust is -16.\n");
    printf("\n");
    printf("-bend - if -16 specified, this makes words big-endian. Default is little-endian.\n");
    printf("\n");
    printf("-maxwinN - maximum lookback window. N is decimal number, which can only be\n");
    printf("           256,512,1024,2048,4096,8192,16384,32768. Default is format-specific\n");
    printf("           maximum window: MegaLZ is 4352, hrum is 4096, hrust is 65536.\n");
    printf("           For given format, window can't be greater than default value\n");
    printf("\n");
    printf("usage:\n");
    printf("mhmt [parameter list] <input filename> [<output filename>]\n");
    printf("\n");
    printf("if no output filename given, filename is appended with \".mlz\", \".hrm\" or \".hst\"\n");
    printf("in accordance with format chosen; for depacking \".dpk\" is appended\n");
    printf("====== mhmt help end ======\n");
    printf("\n");
}
void dump_config(void)
{
    printf("Configuration review:\n");
    printf("\n");
    printf("Pack format: ");

    if( wrk.packtype==PK_MLZ )
        printf("MegaLZ.\n");

    else if( wrk.packtype==PK_HRM )
        printf("Hrum3.5\n");

    else if( wrk.packtype==PK_HST )
        printf("Hrust1.x\n");

    else
        printf("unknown.\n"); // this should be actually never displayed

    printf("Mode:        ");

    if( wrk.mode )
        printf("depacking.\n");

    else
        printf("packing.\n");

    if( !wrk.mode ) {
        printf("Pack coding: ");

        if( wrk.greedy )
            printf("greedy (sub-optimal but faster).\n");

        else
            printf("optimal (slower).\n");
    }

    if( wrk.zxheader ) {
        printf("Header for old ZX ");

        if( wrk.packtype==PK_HRM )
            printf("hrum3.5 ");

        else if( wrk.packtype==PK_HST )
            printf("hrust1.x ");

        printf("depackers is on.\n");
    }

    if( wrk.wordbit ) {
        printf("Bitstream is grouped in words -\n");

        if( wrk.bigend ) {
            printf(" words are big-endian, %s","INCOMPATIBLE with old ZX depackers!\n");

        } else {
            printf(" words are little-endian, ");

            if( (wrk.packtype==PK_HRM) || (wrk.packtype==PK_HST) )
                printf("compatible with old ZX depackers.\n");

            else
                printf("INCOMPATIBLE with old ZX depackers!\n");
        }

    } else {
        printf("Bitstream is grouped in bytes -\n");

        if( wrk.packtype==PK_MLZ )
            printf(" compatible with old ZX depackers.\n");

        else
            printf(" INCOMPATIBLE with old ZX depackers!\n");
    }

    printf("Maximum lookback window size is %d bytes.\n\n",wrk.maxwin);
    // files
    printf("Input file \"%s\" (%d bytes) successfully loaded.\n", wrk.fname_in, wrk.inlen);
    printf("Output file \"%s\" created.\n", wrk.fname_out );
//	...more info...?
}
// create output filename, open files, load input file in memory
// returns 1 if no errors, otherwise zero
ULONG do_files(void)
{
    char * pack_ext;
    char * depk_ext;
    LONG ext_pos;

    // if there is no output filename, create it
    if( !wrk.fname_out ) {
        depk_ext = ".dpk";

        if( wrk.packtype==PK_MLZ )
            pack_ext = ".mlz";

        else if( wrk.packtype==PK_HRM )
            pack_ext = ".hrm";

        else if( wrk.packtype==PK_HST )
            pack_ext = ".hst";

        else
            pack_ext = ".pak"; // all have the same size, as well as depk_ext - 4 bytes!

        wrk.fname_out = (char *)malloc( 5 + strlen(wrk.fname_in) );

        if( !wrk.fname_out ) {
            printf("Can't allocate memory for output filename!\n");
            return 0;
        }

        strcpy(wrk.fname_out, wrk.fname_in);

        if( !wrk.mode ) { // packing
            strcat(wrk.fname_out, pack_ext);

        } else { // depacking
            ext_pos = strlen( wrk.fname_out ) - 4;

            if( (ext_pos>=0) && (!strcmp(&wrk.fname_out[ext_pos], pack_ext)) )
                strcpy( &wrk.fname_out[ext_pos], depk_ext );

            else
                strcat( wrk.fname_out, depk_ext );
        }
    }

    //open files
    wrk.file_in=fopen(wrk.fname_in,"rb");

    if(!wrk.file_in) {
        printf("Cannot open input file \"%s\"!\n",wrk.fname_in);
        return 0;
    }

    wrk.file_out=fopen(wrk.fname_out,"wb");

    if(!wrk.file_out) {
        printf("Cannot create output file \"%s\"!\n",wrk.fname_out);
        return 0;
    }

    // get length of input file
    if( fseek(wrk.file_in,0,SEEK_END) ) {
        printf("Cannot fseek() input file \"%s\"!\n",wrk.fname_in);
        return 0;
    }

    wrk.inlen=(ULONG)ftell(wrk.file_in);

    if( wrk.inlen==(ULONG)(-1L)  ) {
        printf("Cannot ftell() length of input file \"%s\"!\n",wrk.fname_in);
        wrk.inlen=0;
        return 0;

    } else if( wrk.inlen<16 ) {
        printf("Input file \"%s\" is smaller than 16 bytes - I won't process it!\n",wrk.fname_in);
        return 0;
    }

    if( fseek(wrk.file_in,0,SEEK_SET) ) {
        printf("Cannot fseek() input file \"%s\"!\n",wrk.fname_in);
        return 0;
    }

    // load input file in mem
    wrk.indata=(UBYTE *)malloc(wrk.inlen);

    if( !wrk.indata ) {
        printf("Cannot allocate %d bytes of memory for loading input file \"%s\"!\n", wrk.inlen, wrk.fname_in);
        return 0;
    }

    if( wrk.inlen!=fread(wrk.indata,1,wrk.inlen,wrk.file_in) ) {
        printf("Cannot successfully load input file \"%s\" in memory!\n",wrk.fname_in);
        return 0;
    }

    return 1;// no errors
}
