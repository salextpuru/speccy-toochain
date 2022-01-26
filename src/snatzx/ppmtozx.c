/* ppmtozx - converts a 256x192 conversion of a speccy screen back to
 *  speccy display file format. (filter)
 *
 * By default it produces a representation which is visually identical
 * to the original but with the maximum number of zero bits, so that
 * it takes less time to send via tape (zero bits take half the time to
 * send that one bits do). However, such output can look really weird
 * if you strip off the attrs; if this is a problem, uncomment the
 * #define below.
 */

/* uncomment to disable bit optimisation */
// #define NO_FLIP

#include <stdio.h>
#include <stdlib.h>

unsigned char image[256*192*3];		/* input image */
unsigned char speccol[256*192];		/* image mapped to speccy col nums */
unsigned char attr[32*24];		/* the speccy attrs */
unsigned char bmp[32*192];		/* the speccy bitmap */
unsigned char fliporig[9],flipped[9];


int main()
{
FILE *in;
unsigned char buf[1024];
int w,h;
int x,y,a,b,r,g,c,f,specy,mask;

in=stdin;

fgets(buf,sizeof(buf),in);
fscanf(in,"%d%d\n",&w,&h);

if(buf[0]!='P')
  {
  printf("Stdin not a PNM file!\n");
  exit(1);
  }

switch(buf[1])
  {
  case '6':
    break;
  default:
    fprintf(stderr,"Sorry, raw PPM files only (\"P6\").\n");
    exit(1);
  }

if(w!=256 || h!=192)
  fprintf(stderr,"Must be 256x192.\n"),exit(1);

fgets(buf,sizeof(buf),in);
if(atoi(buf)!=255)
  fprintf(stderr,"Maxval must be 255 (run through `pnmdepth 255').\n"),exit(1);

fread(image,3,256*192,in);

if(in!=stdin) fclose(in);

for(y=0;y<192;y++)
  {
  for(x=0;x<256;x++)
    {
    r=image[(y*256+x)*3  ];
    g=image[(y*256+x)*3+1];
    b=image[(y*256+x)*3+2];
    
    /* BFI anyone? */
    c=0;
    if(r==0 && g==0 && b!=0) c=1;
    if(r!=0 && g==0 && b==0) c=2;
    if(r!=0 && g==0 && b!=0) c=3;
    if(r==0 && g!=0 && b==0) c=4;
    if(r==0 && g!=0 && b!=0) c=5;
    if(r!=0 && g!=0 && b==0) c=6;
    if(r!=0 && g!=0 && b!=0) c=7;
    if(r>240 || g>240 || b>240) c+=8;
    
    speccol[y*256+x]=c;
    }
  }

/* map colours to attrs and generate matching bitmaps */
specy=0;
for(y=0;y<24;y++)
  {
  for(x=0;x<32;x++)
    {
    int histo[16],outc[16],idx,fg,bg,fgbr,bgbr,tmp;
    
    for(f=0;f<16;f++) histo[f]=outc[f]=0;
    
    for(b=0;b<8;b++)
      for(a=0;a<8;a++)
        histo[speccol[(y*8+b)*256+x*8+a]]++;
    
    idx=0;
    for(f=0;f<16;f++)
      {
      if(histo[f])
        {
        outc[idx++]=f;
        if(idx==3)
          fprintf(stderr,
           "warning: more than 2 colours in square (%d,%d), data lost!\n",x,y);
        }
      }
    
    bg=outc[0]; fg=outc[1];
    if((bg&7)>(fg&7)) tmp=bg,bg=fg,fg=tmp;
    
    bgbr=(bg>>3); bg&=7;
    fgbr=(fg>>3); fg&=7;
    
    /* sort out BRIGHT - both must be either on or off */
    if(bgbr+fgbr==1)
      {
      /* but black can be either, which complicates matters...
       * bg is the darker, so that'll be black if either is.
       * (fg could also be black, but that's unimportant.)
       * black is always set to non-bright in speccol[], so
       * we need to make bg `bright' if black, else give an
       * error.
       */
      
      if(bg==0)
        bgbr++;
      else
        fprintf(stderr,
         "warning: impossible BRIGHT mix in square (%d,%d), data lost!\n",x,y);
      }
    
    /* make the attr */
    attr[y*32+x]=fliporig[0]=fgbr*64+8*bg+fg;
    
    /* now make this char's bitmap according to the attr. */
    bg=8*bgbr+bg; fg=8*fgbr+fg;
    
    for(b=0;b<8;b++)
      {
      for(a=0,tmp=0,mask=128;a<8;a++,mask>>=1)
        {
        c=speccol[(y*8+b)*256+x*8+a];
        if(c==fg) tmp|=mask;
        }
      
      bmp[(specy+8*b)*32+x]=fliporig[b+1]=~tmp;
      }

#ifndef NO_FLIP    
    /* now see if flipping fg/bg reduces the number of `1' bits.
     * I do this to reduce the time taken to send via tape. (Zero bits
     * are sent in half the time one bits are.)
     */
    flipped[0]=((fliporig[0]&0xc0)|((fliporig[0]>>3)&7)|8*(fliporig[0]&7));
    for(f=1;f<9;f++) flipped[f]=~fliporig[f];
    for(f=a=b=0;f<9;f++)
      {
      for(c=0,mask=128;c<8;c++,mask>>=1)
        {
        a+=((fliporig[f]&mask)?1:0);
        b+=((flipped [f]&mask)?1:0);
        }
      }

    if(a>b)
      {
      /* flip fg/bg. */
      attr[y*32+x]=flipped[0];
      for(f=0;f<8;f++)
        bmp[(specy+8*f)*32+x]=~flipped[f+1];
      }
#endif	/* !NO_FLIP */
    }
  
  specy++;
  if(specy==8) specy=64;
  if(specy==72) specy=128;
  }


/* now finally write as speccy screen on stdout */

fwrite(bmp,32,192,stdout);
fwrite(attr,32,24,stdout);

exit(0);
}
