/* z80v3to2 - convert Z80 v3 file to Z80 v2 file
 * public domain by Russell Marks (mr216@gre.ac.uk)
 */

#include <stdio.h>

unsigned char buf[16384];

main(argc,argv)
int argc;
char **argv;
{
FILE *in,*out;
int tmp;

if(argc!=3) {printf("usage: z80v3to2 v3file.z80 v2file.z80\n"); exit(1);}
if((in=fopen(argv[1],"rb"))==NULL) {printf("Couldn't open file.\n"); exit(1);}
fread(buf,1,30,in);
if(buf[6]+buf[7]!=0) {printf("Z80 v1.xx file!\n"); exit(1);}
tmp=fgetc(in); tmp+=256*fgetc(in);
if(tmp!=54) {printf("Z80 v2.xx file!\n"); exit(1);}
if((out=fopen(argv[2],"wb"))==NULL) {printf("Couldn't open file.\n"); exit(1);}
fwrite(buf,1,30,out); fputc(23,out); fputc(0,out);
fread(buf,1,23,in);
buf[0]-=((buf[0]>=2)?1:0); if(buf[0]==6) buf[0]=3;
fwrite(buf,1,23,out); fread(buf,1,54-23,in);
while((tmp=fread(buf,1,sizeof(buf),in))>0)
  fwrite(buf,1,tmp,out);

fclose(in); fclose(out);
exit(0);
}
