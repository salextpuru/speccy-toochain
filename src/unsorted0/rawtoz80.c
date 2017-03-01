/* rawtoz80 - makes 128k Z80 given suitable raw v2/v3 header and page data.
 * originally written for use by bigv3-make, but may be useful more generally.
 * Z80 generating code nicked from my 128k xz80 hack.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define Z80_V2_HEADER_SIZE	55

/* the Z80 v2 header */
unsigned char z80hed[Z80_V2_HEADER_SIZE];

/* RAM in page number order; 0, 1, 2, ... 7 */
unsigned char specmem[128*1024];

#define PAGE_OFFSET(x) ((x)<<14)


main(argc,argv)
int argc;
char **argv;
{
FILE *in,*out;
char outfile[256];
int expect,tmp,p;
int pc,sp,sevenffd;
int f;

if(argc!=3) {usage(); exit(1);}

if((in=fopen(argv[1],"rb"))==NULL) die("Couldn't open file");

if(fread(z80hed,1,Z80_V2_HEADER_SIZE,in)!=Z80_V2_HEADER_SIZE)
  die("Couldn't read Z80 header");

/* PC must be zero, or else this isn't a V2+ file */
if(z80hed[6] || z80hed[7]) die("Not a Z80 v2.x or v3.x file header");

if(z80hed[12]==255) z80hed[12]=1;

/* check that length of 2nd header block is 23 or 54 */
if((z80hed[30]!=23 && z80hed[30]!=54) || z80hed[31]!=0 ||
   z80hed[34]<3) die("Not a 128K v2/v3 header");

/* Z80 v3 messes around with the machine spec numbers (wow,
 * that was a sensible decision) so kludge around that here
 */
if(z80hed[30]==54 && z80hed[34]==3) die("Not a 128K v2/v3 header");

/* ignore any extra stuff in the V3 header */
fclose(in);


/* read pages in */
if((in=fopen(argv[2],"rb"))==NULL) die("Couldn't open file");

fread(specmem+PAGE_OFFSET(5),1,16384,in);
fread(specmem+PAGE_OFFSET(2),1,16384,in);
fread(specmem+PAGE_OFFSET(0),1,16384,in);
fread(specmem+PAGE_OFFSET(1),1,16384,in);
fread(specmem+PAGE_OFFSET(3),1,16384,in);
fread(specmem+PAGE_OFFSET(4),1,16384,in);
fread(specmem+PAGE_OFFSET(6),1,16384,in);
fread(specmem+PAGE_OFFSET(7),1,16384,in);
fclose(in);

/* write Z80 file */
out=stdout;
z80save128(out);

exit(0);
}


die(str)
char *str;
{
fprintf(stderr,"%s.\n",str);
exit(1);
}


usage()
{
printf("usage: rawtoz80 z80_header raw_page_data >out.z80\n");
printf("z80_header should be a valid Z80 v2/v3 header (for 128k Z80).\n");
printf("raw_page_data should be RAM page data in order 5,2,0,1,3,4,6,7.\n");
}



/* remainder ripped from my 128k xz80 hack */

/* save a Z80 v2.x 128k snap - must be in 128k mode */
z80save128(fp)
FILE *fp;
{
   unsigned char outbuf[16384];
   int i,tmp;

   fwrite(z80hed,1,Z80_V2_HEADER_SIZE,fp);   
   
   /* now the memory */
   for(i=0;i<8;i++){
      int count=0;
      unsigned char *src=specmem+i*16384,*dst=outbuf;
      int old=-1,c,f,g,run=0;
      
      /* we do the compression into outbuf before writing anything
       * to the file.
       */
      for(f=0;f<16385;f++){
         if(f==16384) c=-1; else c=*src++;
         if(old==c){
           run++;
         } else {
           if(run>1){
              if(run>=5 || old==0xed){
                 if(dst>outbuf && dst[-1]==0xed){
                    *dst++=old;
                    run--;
                 }
                 while(run>0){
                    g=((run>255)?255:run);
                    *dst++=0xed; *dst++=0xed;
                    *dst++=g;    *dst++=old;
                    run-=g;
                 }
              } else {
                 for(g=0;g<run;g++) *dst++=old;
              }
           } else {
              if(old!=-1) *dst++=old;
           }
           run=1;
         }
         old=c;
      }
      
      /* and actually write the compressed data */
      count=dst-outbuf;
      fputc(count%256,fp);
      fputc(count/256,fp);
      fputc(i+3,fp);
      fwrite(outbuf,1,count,fp);
   }
}
