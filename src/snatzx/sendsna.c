/* sendsna - based on zxsend, and sendsna shell script
 * by Russell Marks 95/6/17
 * added FAST_OUTPUT define 961024
 *
 * Converts a .SNA to an 8kHz sample.
 * example usage:
 *   sendsna foo.sna /dev/dsp		send via soundcard on Linux
 *   sendsna -u foo.sna /dev/audio	send via audio dev on Sun
 *   sendsna foo.sna foo.voc		write .VOC file on DOS
 *
 * NB: writing a .VOC file is the default behaviour on DOS. To
 * override and write a raw unsigned sample use '-r'. To get a
 * .VOC file on other systems, use '-v'.
 *
 * Copy/use/modify as you wish but leave this comment intact.
 */

#include <stdio.h>
#include <string.h>
#ifdef linux
#include <sys/soundcard.h>
#endif


/* see zxsend.c for what this means (only applies to Linux) */
/*#define FAST_OUTPUT*/


struct tapsig {
  int bottom,middle,weird,top;
  };

struct tapsig unsign={0x00,0x80,0xAA,0xFF};
struct tapsig ulaw  ={0x02,0xFF,0x9A,0x80};

struct tapsig *tap=&unsign;

#define FMT_RAW		1	/* raw output, unsigned or ulaw */
#define FMT_VOC		2	/* .VOC file */

#ifndef MSDOS
int outfmt=FMT_RAW;
#else
int outfmt=FMT_VOC;
#endif

#define ONETIME  4		/* num. samples for one bit */
#define ZEROTIME 2		/* num. samples for zero bit */

int checkbyte,basicskip,verbose=0;
unsigned char specreg[27];	/* registers etc. from sna */
unsigned char specmem[49152];	/* the memory snapshot */

/* the basic loader program:
 * 10 LOAD "" CODE
 * 20 RANDOMIZE USR 16416
 */
#define BASLOADER_SIZE	27
unsigned char basloader[BASLOADER_SIZE]={
  0x00,0x0A,0x05,0x00,0xEF,0x22,0x22,0xAF,
  0x0D,0x00,0x14,0x0E,0x00,0xF9,0xC0,0x31,
  0x36,0x34,0x31,0x36,0x0E,0x00,0x00,0x20,
  0x40,0x00,0x0D
  };

/* the m/c loader program which does the rest - source in sendsna-loader.z */
#define BINLOADER_SIZE	102
unsigned char binloader[BINLOADER_SIZE]={
  0x31,0x1E,0x40,0xDD,0x21,0xA5,0x40,0x11,
  0x5B,0xBF,0x3E,0xFF,0x37,0xCD,0x56,0x05,
  0x30,0xF1,0xF3,0xED,0x5B,0xA8,0x40,0xED,
  0x4B,0xAA,0x40,0x2A,0xAC,0x40,0xE5,0xF1,
  0x2A,0xA6,0x40,0xD9,0xDD,0x21,0xA5,0x40,
  0xDD,0x7E,0x00,0xED,0x47,0xED,0x5E,0xDD,
  0x7E,0x19,0xFE,0x01,0x20,0x02,0xED,0x56,
  0xDD,0x7E,0x13,0xCB,0x57,0x20,0x04,0xAF,
  0x32,0x84,0x40,0xDD,0x7E,0x1A,0xD3,0xFE,
  0x2A,0xBA,0x40,0xE5,0xF1,0x2A,0xAE,0x40,
  0xED,0x5B,0xB0,0x40,0xED,0x4B,0xB2,0x40,
  0xFD,0x2A,0xB4,0x40,0xDD,0x2A,0xB6,0x40,
  0xED,0x7B,0xBC,0x40,0xFB,0xC9
  };



main(argc,argv)
int argc;
char **argv;
{
FILE *in,*out;
int f;
long lenpos,samplen;
char specname[11],*ptr;

if(argc<3 || argc>4) usage();
if(argc==4)
  if(*argv[1]=='-')
    {
    switch(argv[1][1])
      {
      case 'r': tap=&unsign; outfmt=FMT_RAW; break;
      case 'u': tap=&ulaw;   outfmt=FMT_RAW; break;
      case 'v': tap=&unsign; outfmt=FMT_VOC; break;
      default:  usage();
      }
    argc--; argv++;
    }
  else
    usage();

/* read sna */

if((in=fopen(argv[1],"rb"))==NULL)
  die("Couldn't open input file.\n");

if(fread(specreg,1,27,in)!=27)
  die("Couldn't read file.\n");

/* just in case, as some old compilers might not like it */
#ifdef MSDOS
if(fread(specmem,1,49152U,in)!=49152U)
#else
if(fread(specmem,1,49152,in)!=49152)
#endif
  die("Couldn't read file.\n");

fclose(in);

/* write output */

if((out=fopen(argv[2],"wb"))==NULL)
  die("Couldn't open output file.\n");

#ifdef linux
#ifdef FAST_OUTPUT
{int spd=10000;
if(strcmp(argv[2],"/dev/dsp")==0 &&
	(f=ioctl(fileno(out),SNDCTL_DSP_SPEED,&spd))==-1)
  fprintf(stderr,"Error setting frequency, carrying on anyway");}
#endif
#endif

/* do VOC header stuff if necessary */
if(outfmt==FMT_VOC)
  {
  fprintf(out,"Creative Voice File\x1A\x1A");	/* header */
  fputc(0,out);
  fprintf(out,"\x0A\x01\x29\x11");
  fputc(1,out);		/* sound data block */
  lenpos=ftell(out);
  fputc(0,out); fputc(0,out); fputc(0,out); /* we fill the length in later */
  fputc(131,out);	/* 8kHz; 256-(1e6/8000) */
  fputc(0,out);		/* no compression, 8-bit data */
  /* the sample data follows... */
  }

/* first construct speccy filename from snap */
if((ptr=strrchr(argv[1],'/'))==NULL)
  if((ptr=strrchr(argv[1],'\\'))==NULL)
    ptr=argv[1];
  else
    ptr++;
else
  ptr++;
for(f=0;f<10;f++) specname[f]=ptr[f];
specname[10]=0;
if((ptr=strrchr(specname,'.'))!=NULL) *ptr=0;

/* now send basic loader with that name */
fprintf(stderr,"writing loader part 1\n");
sendfile(out,specname,basloader,BASLOADER_SIZE,0,0);

/* next the m/c loader */
fprintf(stderr,"writing loader part 2\n");
sendfile(out,"loader",binloader,BINLOADER_SIZE,3,0x4020);
 
/* finally, the registers followed by all but the first 192 bytes of
 * the snap. We copy the 27 bytes of registers into the appropriate
 * place then send it.
 */
memcpy(specmem+192-27,specreg,27);
fprintf(stderr,"writing data\n");
verbose=1;
sendfile(out,"data",specmem+192-27,0xBF5B,3,0x40A5);

if(outfmt==FMT_VOC)
  {
  samplen=ftell(out)-30;
  fputc(0,out);		/* terminator block */
  fseek(out,lenpos,SEEK_SET);
  fputc((int)(samplen&0xff),out);
  fputc((int)(samplen>>8),out);
  fputc((int)(samplen>>16),out);
  }

fclose(out);

fprintf(stderr,"done.\n");

exit(0);
}


usage()
{
printf("usage: sendsna [-ruv] infile.sna outfile\n\n");
printf("        -r      write raw unsigned sample file\n");
printf("        -u      write raw u-law sample file\n");
printf("        -v      write .VOC file\n\n");
printf("default is to write ");
#ifdef MSDOS
printf(".VOC file");
#else
printf("raw unsigned sample file");
#endif
printf("\n");
exit(1);
}


die(mes)
char *mes;
{
fprintf(stderr,mes);
exit(1);
}


sendfile(audio,speccyfilename,dataptr,length,filetype,startplace)
FILE *audio;
char *speccyfilename;
unsigned char *dataptr;
unsigned int length;
int filetype,startplace;
{
FILE *in;
int spd,a,f;
char filename[11];
int c;

firsttone(audio,0x1F80);        /* header */
sendbyte(audio,0);
checkbyte=0;

/* the header itself */
sendbyte(audio,filetype);

strcpy(filename,speccyfilename);
for(f=strlen(speccyfilename);f<10;f++)
  filename[f]=32;
  
for(f=0;f<10;f++)
  sendbyte(audio,filename[f]);

sendbyte(audio,length%256);
sendbyte(audio,length/256);

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
  basicskip=length;			/* assume no variables */
  sendbyte(audio,basicskip%256);
  sendbyte(audio,basicskip/256);
  }
sendbyte(audio,checkbyte);

/* wait a second */
c=tap->middle;
for(f=0;f<8000;f++)
  fputc(c,audio);

firsttone(audio,0x0C98);        /* data */
sendbyte(audio,0xFF);

checkbyte=0xFF;
for(f=0;f<length;f++)
  {
  if((verbose)&&((f%256==0)||(f==length-1)))
    pcntbar(f,length);
  sendbyte(audio,dataptr[f]);
  }
  
sendbyte(audio,checkbyte);

c=tap->middle;
for(f=0;f<4000;f++)
  fputc(c,audio);

if(verbose) fprintf(stderr,"\n");
}


pcntbar(done,outof)
unsigned int done,outof;
{
long f,stp,timeleft;

stp=(long)done*71/(long)outof;
timeleft=(((long)outof-(long)done)<<3)/1400;  /* approximate time left */
fprintf(stderr,"%0.2ld:%0.2ld [",timeleft/60,timeleft%60);
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
int c;
int f,count;

c=tap->bottom;
for(count=0;count<length;count++)
  {
  for(f=0;f<5;f++)
    fputc(c,audio);
    
  if(c==tap->bottom) c=tap->top; else c=tap->bottom;
  }

/* I think this is pretty important, which is weird */
fputc(tap->bottom,audio);
fputc(tap->weird, audio);
fputc(tap->top,   audio);
}


sendbyte(audio,b)
FILE *audio;
int b;
{
int c;
int f,g,n,mask;

mask=0x80;

checkbyte^=b;
for(g=0;g<8;g++)
  {
  c=tap->bottom;
  n=(b&mask)?ONETIME:ZEROTIME;
  for(f=0;f<n;f++) fputc(c,audio);
  c=tap->top;
  for(f=0;f<n;f++) fputc(c,audio);
  mask>>=1;
  }
}
