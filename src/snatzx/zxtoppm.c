#include <stdio.h>

/* values nicked from xzx */
#define DARK		0xBF
#define LITE		0xFF


char idx[256][192];

char pal[16][3]={
/* not bright */
  {   0,   0,   0},
  {   0,   0,DARK},
  {DARK,   0,   0},
  {DARK,   0,DARK},
  {   0,DARK,   0},
  {   0,DARK,DARK},
  {DARK,DARK,   0},
  {DARK,DARK,DARK},
/* bright */
  {   0,   0,   0},
  {   0,   0,LITE},
  {LITE,   0,   0},
  {LITE,   0,LITE},
  {   0,LITE,   0},
  {   0,LITE,LITE},
  {LITE,LITE,   0},
  {LITE,LITE,LITE},
  };

unsigned char pic[192][256];
unsigned char attrbuf[32];
int specy[192];


main()
{
FILE *in,*out;
int x,y,sy,i,v,n,c;
int a,b,f,fg,bg;

in=stdin; out=stdout;

for(y=0;y<192;y++)
  {
  /* y=speccy y pos, specy[y]=onscreen y pos - no, it doesn't make sense */
  specy[y]=8*(y%8)+1*((y%64)>>3)+64*(y/64);
  }

for(y=0;y<192;y++)
  {
  sy=specy[y];
  for(x=0;x<32;x++)
    {
    n=128;
    v=0;
    c=fgetc(in);
    for(i=0;i<8;i++)
      {
      pic[sy][x*8+i]=((c&n)?1:0);
      n>>=1;
      }
    }
  }

/* 76543210
 * FbPAPink */
for(y=0;y<24;y++)
  for(x=0;x<32;x++)
    {
    c=fgetc(in);
    fg=((c>>6)<<3)|(c&7);
    bg=((c>>6)<<3)|((c>>3)&7);
    for(b=0;b<8;b++)
      for(a=0;a<8;a++)
        if(pic[(y<<3)|b][(x<<3)|a]==1)
          pic[(y<<3)|b][(x<<3)|a]=fg;
        else
          pic[(y<<3)|b][(x<<3)|a]=bg;
    }

printf("P6\n256 192\n%d\n",LITE);

for(y=0;y<192;y++)
  for(x=0;x<256;x++)
    {
    putchar(pal[pic[y][x]][0]);
    putchar(pal[pic[y][x]][1]);
    putchar(pal[pic[y][x]][2]);
    }

fclose(out);
fclose(in);

exit(0);
}

