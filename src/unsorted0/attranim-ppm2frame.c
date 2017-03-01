#include <stdio.h>

unsigned char image[96*192*3],dith[96*192*3];
unsigned char out[96*192],out2[12*192];
unsigned char ppmbuf[96*3];
int fg[64],bg[64];
int specy[192];

int pal[8*3]={
    0,  0,  0,
    0,  0,255,
  255,  0,  0,
  255,  0,255,
    0,255,  0,
    0,255,255,
  255,255,  0,
  255,255,255
  };
    


int main()
{
int a,x,y,f,r,g,b;
int col,dark,light;
unsigned char *ptr;
int i,n,v,sy;
int addr=0x5800+10;
unsigned char buf[32];

for(f=0;f<14;f++) getchar();

fread(image,1,64*24*3,stdin);

for(y=0;y<24;y++)
  {
  for(x=0;x<64;x+=2)
    {
    r=image[(y*64+x)*3  ];
    g=image[(y*64+x)*3+1];
    b=image[(y*64+x)*3+2];
    if(r==0 && g==0 && b==0) col=0;
    if(r==0 && g==0 && b!=0) col=1;
    if(r!=0 && g==0 && b==0) col=2;
    if(r!=0 && g==0 && b!=0) col=3;
    if(r==0 && g!=0 && b==0) col=4;
    if(r==0 && g!=0 && b!=0) col=5;
    if(r!=0 && g!=0 && b==0) col=6;
    if(r!=0 && g!=0 && b!=0) col=7;
    i=col;
    r=image[(y*64+x+1)*3  ];
    g=image[(y*64+x+1)*3+1];
    b=image[(y*64+x+1)*3+2];
    if(r==0 && g==0 && b==0) col=0;
    if(r==0 && g==0 && b!=0) col=1;
    if(r!=0 && g==0 && b==0) col=2;
    if(r!=0 && g==0 && b!=0) col=3;
    if(r==0 && g!=0 && b==0) col=4;
    if(r==0 && g!=0 && b!=0) col=5;
    if(r!=0 && g!=0 && b==0) col=6;
    if(r!=0 && g!=0 && b!=0) col=7;
    /* paper left, ink right */
    buf[x/2]=(i*8+col);
    }
  
  for(x=0;x<32;x+=4)
    {
    putchar((buf[x+0]<<2)|(buf[x+1]>>4));
    putchar((buf[x+1]<<4)|(buf[x+2]>>2));
    putchar((buf[x+2]<<2)|(buf[x+3]));
    }
  }
}
