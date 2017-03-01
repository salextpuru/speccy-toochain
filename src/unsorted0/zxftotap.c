/* convert zxf file to tap
 * usage: zxftotap <foo.zxf >foo.tap
 * PD by RJM 970610
 */

#include <stdio.h>
#include <string.h>

unsigned char buf[65536];

main()
{
int ofs,f,siz,chk;
FILE *out;

fread(buf,1,4,stdin);

if(strncmp(buf,"ZXF1",4)!=0)
  fprintf(stderr,"not a zxf file.\n"),exit(1);

fread(buf,1,17,stdin);
putchar(19); putchar(0);
putchar(chk=0);
for(f=0;f<17;f++) chk^=buf[f];
fwrite(buf,1,17,stdout);
putchar(chk);

siz=2+fread(buf,1,65536,stdin);
putchar(siz&255); putchar(siz>>8);
siz-=2;
putchar(chk=255);
for(f=0;f<siz;f++) chk^=buf[f];
fwrite(buf,1,siz,stdout);
putchar(chk);

exit(0);
}
