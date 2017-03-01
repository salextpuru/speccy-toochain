/* mf2sna - convert MF3 48k snaps to .SNA
 * PD by RJM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long getword();


unsigned char specmem[0xC000]; /* 48K speccy's memory */
unsigned char tempmem[0x5B3];

struct
  {
  unsigned char pc_lo,pc_hi,iy_lo,iy_hi,ix_lo,ix_hi;
  unsigned char ac,ab,ae,ad,al,ah,af,aa;
  unsigned char nc,nb,ne,nd,mb,r,mi,i,nl,nh,nf,na,sp_lo,sp_hi;
  } specregs;


main(argc,argv)
int argc;
char **argv;
{
FILE *in,*out;

if(argc!=4)
  printf("usage: mf2sna infile.mf outfile.sna word@72h\n");
else
  {
  if((in=fopen(argv[1],"rb"))==NULL)
    printf("Couldn't open '%s'.\n",argv[1]);
  else
    {
    if((out=fopen(argv[2],"wb"))==NULL)
      printf("Couldn't open '%s'.\n",argv[2]);
    else
      {
      process(in,out,(long)atoi(argv[3]));
      fclose(out);
      }
    fclose(in);
    }
  }
}


process(in,out,magic)
FILE *in,*out;
long magic;
{
long sp,pc;

fread(&(specmem[0x1FE0]),(unsigned)magic,1,in);
fread(&(specmem[0x0000]),(unsigned)getword(0x1FFCL),1,in);

expand(getword(0x1FFEL)-0x4000L,0xC000L,getword(0x1FF6L)-0x4000L);
expand(getword(0x1FFCL),0x1B00L,0L);

memcpy(tempmem,specmem,0x05B3);
fread(specmem,0x05B3,1,in);
memcpy(&specregs,specmem+0x202a,0x001c);
memcpy(specmem+0x1B00,specmem,0x05B3);
memcpy(specmem,tempmem,0x05B3);

writeregs(out);
fwrite(&(specmem[0]),32767,1,out);
fwrite(&(specmem[32767]),16385,1,out);
}


writeregs(out)
FILE *out;
{
fputc(specregs.i ,out);
fputc(specregs.al,out);
fputc(specregs.ah,out);
fputc(specregs.ae,out);
fputc(specregs.ad,out);
fputc(specregs.ac,out);
fputc(specregs.ab,out);
fputc(specregs.af,out);
fputc(specregs.aa,out);
fputc(specregs.nl,out);
fputc(specregs.nh,out);
fputc(specregs.ne,out);
fputc(specregs.nd,out);
fputc(specregs.nc,out);
fputc(specregs.nb,out);
fputc(specregs.iy_lo,out);
fputc(specregs.iy_hi,out);
fputc(specregs.ix_lo,out);
fputc(specregs.ix_hi,out);
if((specregs.mi&4)) fputc(4,out); else fputc(0,out);	/* ei/di */
fputc(specregs.r,out);
fputc(specregs.nf,out);
fputc(specregs.na,out);
fputc(specregs.sp_lo,out);
fputc(specregs.sp_hi,out);
if((specregs.mi&1)) fputc(2,out); else fputc(1,out); /* int. mode */
fputc(0,out);  /* border colour */
}


expand(from,to,stopat)
long from,to,stopat;
{
int a;
long f,c;

from--; to--; stopat--;

printf("%lX %lX %lX\n",(long)from,(long)to,(long)stopat);

while(from!=stopat)
  {
  if((specmem[from]==0xCB)&&(specmem[from-1]==0xED)&&(specmem[from-2]==0x37))
    {
    from-=3;
    a=specmem[from--];
    c=256*specmem[from--];
    c+=specmem[from--];
    for(f=0;f<c;f++)
      specmem[to--]=a;
    }
  else
    {
    specmem[to]=specmem[from];
    from--;
    to--;
    }
  }
}


long getword(pos)
long pos;
{
long a;

a=(long)specmem[pos]+256L*(long)specmem[pos+1L];
return(a);
}
