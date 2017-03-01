/* infosna - give info from header of .sna file.
 * it reports SP as SP+2, as the .sna format stores PC on stack.
 * PC is reported separately by looking at the RAM.
 * PD by Russell Marks 961112
 *
 * NB: there may be problems compiling this on a 16-bit dos C compiler.
 * Not sure, haven't tried it.
 */

#include <stdio.h>


#define GETWORD(x)	(*(buf+(x))+((*(buf+(x)+1))<<8))


main(argc,argv)
int argc;
char **argv;
{
unsigned char buf[27];
FILE *in;

if(argc!=2 || (in=fopen(argv[1],"rb"))==NULL)
  {
  printf("usage: infosna snapshot_file\n");
  exit(1);
  }

fread(buf,1,27,in);

printf("AF =%04X : BC =%04X : DE =%04X : HL =%04X : IX =%04X : IY =%04X\n",
  GETWORD(21),GETWORD(13),GETWORD(11),GETWORD(9),GETWORD(17),GETWORD(15));
printf("AF'=%04X : BC'=%04X : DE'=%04X : HL'=%04X :",
  GETWORD(7),GETWORD(5),GETWORD(3),GETWORD(1));
printf("  I = %02X  :  R = %02X\n",buf[0],buf[20]);
printf("ints %s :   IM %d   : SP =%04X : PC =",
  (buf[19]&4)?"on ":"off",buf[25],GETWORD(23)+2);

if(GETWORD(23)<0x4000)
  printf("<stack in ROM, can't read PC>\n");
else
  {
  fseek(in,GETWORD(23)-0x4000+27,0);
  fread(buf,1,2,in);
  printf("%04X\n",GETWORD(0));
  }
  
exit(0);
}
