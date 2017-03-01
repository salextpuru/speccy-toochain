/* zxsend.c - public domain by Russell Marks 94/1/24
 * /dev/audio support 94/5/12
 * restructured 95/5/6
 *
 * Send a file (via a sound card) to a ZX Spectrum to read with LOAD.
 * If not on a Linux box or a Sun, write to a file instead.
 */

#define ZXSEND_VERSION	"1.0"


#include <stdio.h>
#include <string.h>
#ifndef MSDOS
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#endif
#include "getoptn.h"

#ifdef __TURBOC__
#define MSDOS 1
#endif


#if defined(linux) || defined(MSDOS)
#define BOTTOM_SIG	0x00
#define MIDDLE_SIG	0x80
#define WEIRD_SIG	0xAA
#define TOP_SIG		0xFF
#else
/* I used sox to mangle a file ub -> ul with the above bytes and got this: */
#define BOTTOM_SIG	0x02
#define MIDDLE_SIG	0xFF
#define WEIRD_SIG	0x9A
#define TOP_SIG		0x80
#endif

#define ONETIME  4
#define ZEROTIME 2


int checkbyte,basicskip,verbose=0;
int file_type=3;	/* code */
int code_address=32768;
int basic_startline=32768;
int vars_offset=0;
#ifdef MSDOS
int use_voc=1;	/* default to using VOC on DOS */
#else
int use_voc=0;
#endif
char speccy_filename[11];
char outfile[256],infile[256];
#ifndef MSDOS
unsigned char filedata[65536];  /* as much as you could possibly save */
#else
unsigned char filedata[49152];	/* good enough in practice */
#endif



main(argc,argv)
int argc;
char **argv;
{
long size;
int filetype;
FILE *in;

speccy_filename[0]=0;
strcpy(outfile,DEFAULT_OUTPUT);

parseoptions(argc,argv);

if((in=fopen(infile,"rb"))==NULL)
  die("Couldn't open input file.\n");

if((size=fread(filedata,1,sizeof(filedata),in))<=0)
  die("Couldn't read input file.\n");

fclose(in);

if(vars_offset)
  basicskip=vars_offset;
else
  basicskip=size;

sendfile(speccy_filename,filedata,size,file_type,
		file_type?code_address:basic_startline);

exit(0);
}


die(mes)
char *mes;
{
fprintf(stderr,mes);
exit(1);
}


sendfile(speccyfilename,dataptr,length,filetype,startplace)
char *speccyfilename;
unsigned char *dataptr;
long length;
int filetype,startplace;
{
FILE *audio;
FILE *in;
int spd,a;
char filename[11];
unsigned char c;
long f,lenpos;


if((audio=fopen(outfile,"wb"))==NULL)
  {
  fprintf(stderr,"Couldn't open `%s'.\n",outfile);
  exit(1);
  }

/* do VOC header stuff if necessary */
if(use_voc)
  {
  fprintf(audio,"Creative Voice File\x1A\x1A");	/* header */
  fputc(0,audio);
  fprintf(audio,"\x0A\x01\x29\x11");
  fputc(1,audio);		/* sound data block */
  lenpos=ftell(audio);
  fputc(0,audio); fputc(0,audio); fputc(0,audio); /* length filled in later */
  fputc(131,audio);		/* 8kHz; 256-(1e6/8000) */
  fputc(0,audio);		/* no compression, 8-bit data */
  /* the sample data follows... */
  }

firsttone(audio,0x1F80);        /* header */
sendbyte(audio,0);
checkbyte=0;

/* the header itself */
sendbyte(audio,filetype);

strcpy(filename,speccyfilename);
a=strlen(filename);
for(f=a;f<10;f++)
  filename[f]=32;
  
for(f=0;f<10;f++)
  sendbyte(audio,filename[f]);

sendbyte(audio,(int)(length%256));
sendbyte(audio,(int)(length/256));

if(filetype!=0)
  {
  /* code */
  sendbyte(audio,startplace%256);
  sendbyte(audio,startplace/256);
  sendbyte(audio,0);
  sendbyte(audio,0);      /* unused */
  }
else
  {
  /* basic */
  sendbyte(audio,startplace%256);
  sendbyte(audio,startplace/256);
  sendbyte(audio,basicskip%256);
  sendbyte(audio,basicskip/256);
  }
sendbyte(audio,checkbyte);

/* wait a second */
for(f=0;f<8000;f++)
  fputc(MIDDLE_SIG,audio);

firsttone(audio,0x0C98);        /* data */
sendbyte(audio,0xFF);

checkbyte=0xFF;
for(f=0;f<length;f++)
  {
  if(verbose && (f%256==0 || f==length-1))
    pcntbar(f,length);
  sendbyte(audio,filedata[f]);
  }
  
sendbyte(audio,checkbyte);

for(f=0;f<4000;f++)
  fputc(MIDDLE_SIG,audio);

if(use_voc)
  {
  long samplen;
  
  samplen=ftell(audio)-30;
  fputc(0,audio);		/* terminator block */
  fseek(audio,lenpos,SEEK_SET);
  fputc((int)(samplen&0xff),audio);
  fputc((int)(samplen>>8),audio);
  fputc((int)(samplen>>16),audio);
  }

fclose(audio);
if(verbose) fprintf(stderr,"\n");
}


pcntbar(done,outof)
long done,outof;
{
long f,stp,timeleft;

stp=done*71/outof;
timeleft=((outof-done)<<3)/1400;  /* approximate time left */
fprintf(stderr,"%02ld:%02ld [",timeleft/60,timeleft%60);
for(f=0;f<stp;f++)
  fprintf(stderr,"#");
for(f=stp;f<70;f++)
  fprintf(stderr,":");
fprintf(stderr,"]\r");
}



firsttone(audio,length)
FILE *audio;
int length;
{
unsigned char c;
int f,count;

c=BOTTOM_SIG;
for(count=0;count<length;count++)
  {
  for(f=0;f<5;f++)
    fputc(c,audio);
    
  if(c==BOTTOM_SIG) c=TOP_SIG; else c=BOTTOM_SIG;
  }

/* I think this is pretty important, which is weird */
fputc(BOTTOM_SIG,audio);
fputc( WEIRD_SIG,audio);
fputc(   TOP_SIG,audio);
}


sendbyte(audio,b)
FILE *audio;
int b;
{
unsigned char c;
int f,g,n,mask;

mask=0x80;

checkbyte^=b;
for(g=0;g<8;g++)
  {
  c=BOTTOM_SIG;
  n=(b&mask)?ONETIME:ZEROTIME;
  for(f=0;f<n;f++) fputc(c,audio);
  c=TOP_SIG;
  for(f=0;f<n;f++) fputc(c,audio);
  mask>>=1;
  }
}



/* cmdline option parsing routine.
 */
parseoptions(argc,argv)
int argc;
char **argv;
{
int done=0;

done=0;
optnerr=0;	/* only needed with a real getopt(), really */


do
  switch(getoptn(argc,argv,"a:bchl:n:o:rvV:"))
    {
    case 'a':	/* code address */
      code_address=atoi(optnarg);
      if(code_address==0 || code_address>65535)
        die("Address must be in the range 1 to 65535.\n");
      break;
    case 'b':	/* basic file */
      file_type=0; break;
    case 'c':	/* output CVF (VOC) file */
      use_voc=1; break;
    case 'h':	/* usage help */
      usage_help();
      exit(1);
    case 'l':
      basic_startline=atoi(optnarg);
      if(basic_startline>9999)
        die("Start line must be in the range 0 to 9999.\n");
      break;
    case 'n':
      if(strlen(optnarg)>10)
        die("Spectrum filename must be 10 chars max.\n");
      strcpy(speccy_filename,optnarg);
      break;
    case 'o':	/* output to a file */
      strcpy(outfile,optnarg);
      break;
    case 'r':	/* output raw file */
      use_voc=0; break;
    case 'v':	/* verbosity (i.e. % complete bar) */
      verbose=1; break;
    case 'V':
      vars_offset=atoi(optnarg);
      if(vars_offset<=0)
        die("VARS offset must be at least slightly reasonable. :-)\n");
      break;
    case '?':
      switch(optnopt)
        {
        case 'a':
          fprintf(stderr,"The 'a' option takes an address arg.\n");
          break;
        case 'l':
          fprintf(stderr,"The 'l' option takes a line number arg.\n");
          break;
        case 'n':
          fprintf(stderr,"The 'n' option takes a Spectrum filename arg.\n");
          break;
        case 'o':
          fprintf(stderr,"The 'o' option takes a filename arg.\n");
          break;
        case 'V':
          fprintf(stderr,"The 'V' option takes offset of VARS as arg.\n");
          break;
        default:
          fprintf(stderr,"Option '%c' not recognised.\n",optnopt);
        }
      exit(1);
    case -1:
      done=1;
    }
while(!done);

if(optnind!=argc-1)	/* if no filename given */
  {
  usage_help();
  exit(1);
  }

strcpy(infile,argv[optnind]);
}


usage_help()
{
printf("Zxsend v%s - Public domain by Russell Marks for improbabledesigns.\n\n",
		ZXSEND_VERSION);

printf("usage: zxsend [-bchrv] [-a address] [-l line] [-n speccy_filename]\n");
printf("              [-o output_file] [-V vars_offset] input_file\n\n");

printf("        -a      set start address of code file (default 32768).\n");
printf("        -b      send input_file as a basic file rather than code.\n");
printf("        -c      output CVF (VOC) file (default on MS-DOS).\n");
printf("        -h      give this usage help.\n");
printf("        -l      set auto-start line of basic file (default none).\n");
printf("        -n      set Spectrum filename (to be given in tape header).");
printf("\n        -o      specify output file (default '%s').\n",
						DEFAULT_OUTPUT);
printf("        -r      output raw 8-bit sample file (usual default).\n");
printf("        -v      verbose; show percent-complete display while sending");
printf("\n	-V	set offset of VARS in basic program.\n\n");
}
