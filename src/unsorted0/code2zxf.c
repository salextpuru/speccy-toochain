#include <stdio.h>

unsigned char kludge[65536];

main(argc,argv)
int argc;
char **argv;
{
int siz,st;

siz=fread(kludge,1,65536,stdin);
fprintf(stderr,"%d bytes.\n",siz);

/* magic, file type (code) and speccy filename */
printf("ZXF1%c%-10s",3,(argc>=2)?argv[1]:"");

/* length */
putchar(siz%256);
putchar(siz/256);

/* start */
st=0;
if(argc==2)
  st=atoi(argv[1]);

if(st<16384)
  st=32768;

putchar(st%256);
putchar(st/256);

/* unused */
putchar(0);
putchar(0);

fwrite(kludge,1,siz,stdout);

exit(0);
}
