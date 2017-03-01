#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MegaLZ_types.h"
#include "MegaLZ_globals.h"

#include "MegaLZ_pack.h"
#include "MegaLZ_depack.h"


char * fname_in;  //filename of input file
char * fname_out; // --//-- of output file

FILE * file_in;  //input file
FILE * file_out; //output file

ULONG inlen; //length of input file

UBYTE * indata; // input data here


ULONG mode; // mode of working:


#define PARSE_ERROR 1
#define PARSE_HELP  2
#define PARSE_WORK  3
#define PARSE_FAIL  4


#define PACK_EXT ".mlz"
#define DEPK_EXT ".dpk"

int parse_input(int,char **);
int main_job(void);

int main(int argc, char* argv[])
{
	int retcode=0;


	// init globals

	fname_in=NULL;
	fname_out=NULL;

	file_in=NULL;
	file_out=NULL;

	inlen=0;
	indata=NULL;




	printf("MegaLZ C packer/depacker v4.89\n");



	switch( parse_input(argc,argv) )
	{
	case PARSE_ERROR:
		printf("\nError in arguments! Try megalz -h for help\n");
		retcode=1;
		break;
	case PARSE_HELP:
		printf("\nUsage:\n megalz -h - this help\n");
		printf(          " megalz [-g] <input filename> [<output filename>] - packing\n");
		printf(          "         -g - greedy coding, default - optimal coding\n");
		printf(          "Greedy coding is significantly faster but yields worse packing\n");
		printf(          " megalz -d <input filename> [<output filename>] - depacking\n");
		printf(          "Default extension for packed file - '.mlz', for depacked - '.dpk'\n");
		printf("\n (c) 2005 MAYhEM\n");
		printf(" Z80 depacker code, packed file format (c) fyrex^mhm\n");
		printf(" This C packer/depacker (c) lvd^mhm\n\n");
		break;
	case PARSE_WORK:
		if( (mode==MODE_PACK_OPTIMAL) || (mode==MODE_PACK_GREEDY) )
		{
			printf("Packing...\n");
		}
		else if( mode==MODE_DEPACK )
		{
			printf("Depacking...\n");
		}
		fflush(stdin);
		retcode=main_job()?0:1;
		break;
	default:
		retcode=1;
	}




	// free globals (if any)

	if(fname_in) free(fname_in);
	if(fname_out) free(fname_out);
	if(file_in) fclose(file_in);
	if(file_out) fclose(file_out);
	if(indata) free(indata);





	if( !retcode ) printf("Done!\n");
	return retcode;
}




int parse_input(int argc,char * argv[])
// parse input arguments
{
	int fname_pos,ext_pos;

	if( (argc>=2)&&(argc<=4) )
	{
		if( !(strcmp(argv[1],"-h")&&strcmp(argv[1],"-H")) )
		{
			return PARSE_HELP;
		}
		else if( !(strcmp(argv[1],"-g")&&strcmp(argv[1],"-G")) )
		{
			fname_pos=2;
			mode=MODE_PACK_GREEDY;
		}
		else if( !(strcmp(argv[1],"-d")&&strcmp(argv[1],"-d")) )
		{
			fname_pos=2;
			mode=MODE_DEPACK;
		}
		else
		{
			fname_pos=1;
			mode=MODE_PACK_OPTIMAL;
		}

		if( fname_pos>=argc ) return PARSE_ERROR;

		// input filename
		fname_in=(char *)malloc(strlen(argv[fname_pos])+1);
		if( !fname_in )
		{
			printf("parse_input(): fname_in malloc failed!\n");
			return PARSE_FAIL;
		}
		strcpy(fname_in,argv[fname_pos]);

		// output filename
		if( (fname_pos+1)<argc )
		{ // we have second filename
			fname_out=(char *)malloc(strlen(argv[fname_pos+1])+1);
			if( !fname_out)
			{
				printf("parse_input(): fname_out malloc failed!\n");
				return PARSE_FAIL;
			}
			strcpy(fname_out,argv[fname_pos+1]);
		}
		else
		{ // no second filename - create from old adding extensions
			fname_out=(char *)malloc(strlen(argv[fname_pos])+1+4);
			if( !fname_out)
			{
				printf("parse_input(): fname_out malloc failed!\n");
				return PARSE_FAIL;
			}
			strcpy(fname_out,argv[fname_pos]);
			if( mode==MODE_DEPACK )
			{
				ext_pos=strlen(fname_out)-4;
				if( !strcmp(&fname_out[ext_pos],PACK_EXT) )
				{
					strcpy(&fname_out[ext_pos],DEPK_EXT);
				}
				else
				{
					strcat(fname_out,DEPK_EXT);
				}
			}
			else
			{ // MODE_PACK_*
				strcat(fname_out,PACK_EXT);
			}
		}
	}
	else
	{
		return PARSE_ERROR;
	}


	return PARSE_WORK;
}




int main_job(void)
{


	// open input file
	file_in=fopen(fname_in,"rb");

	if(!file_in)
	{
		printf("Cannot open input file \"%s\"!\n",fname_in);
		return 0;
	}


	// open output file
	file_out=fopen(fname_out,"wb");
	if(!file_out)
	{
		printf("Cannot create output file \"%s\"!\n",fname_out);
		return 0;
	}




	//get length of file
	if( fseek(file_in,0,SEEK_END) )
	{
		printf("Cannot fseek input file \"%s\"!\n",fname_in);
		return 0;
	}

	inlen=ftell(file_in);
	if( inlen==(-1L)  )
	{
		printf("Cannot ftell() length of input file \"%s\"!\n",fname_in);
		inlen=0;
		return 0;
	}
	else if( inlen<4 )
	{
		printf("Input file \"%s\" is smaller than 4 bytes - I won't process it!\n",fname_in);
		return 0;
	}

	//return ptr to beginning
	if( fseek(file_in,0,SEEK_SET) )
	{
		printf("Cannot fseek() input file \"%s\"!\n",fname_in);
		return 0;
	}




	//alloc mem for file and load it into mem

	indata=(UBYTE*)malloc(inlen);
	if( !indata )
	{
		printf("Cannot allocate memory for loading input file \"%s\"!\n",fname_in);
		return 0;
	}

	if( inlen!=fread(indata,1,inlen,file_in) )
	{
		printf("Cannot successfully load input file \"%s\" into mem!\n",fname_in);
		return 0;
	}




	// now file loaded, pack it

	if( (mode==MODE_PACK_OPTIMAL)||(mode==MODE_PACK_GREEDY) )
	{
		if( !pack() )
		{
			printf("main_job(): pack() failed!\n");
			return 0;
		}
	}
	else if( mode==MODE_DEPACK )
	{
		if( !depack() )
		{
			printf("main_job(): depack() failed!\n");
			return 0;
		}
	}


	return 1;
}

