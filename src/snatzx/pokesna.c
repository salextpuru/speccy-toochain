#include <stdio.h>
#include <string.h>


main(argc,argv)
int argc;
char **argv;
{
FILE *sna;
int addr,value;

if(argc!=4)
  {
  printf("usage: pokesna sna_file address value\n");
  exit(1);
  }

if((sna=fopen(argv[1],"r+"))==NULL)
  {
  printf("Couldn't open file.\n");
  exit(1);
  }

if(!strncmp(argv[2],"0x",2))
  sscanf(argv[2]+2,"%x",&addr);
else
  addr=atoi(argv[2]);

if(!strncmp(argv[3],"0x",2))
  sscanf(argv[3]+2,"%x",&value);
else
  value=atoi(argv[3]);

if(fseek(sna,addr-0x4000+0x1B,SEEK_SET)==-1)
  {
  printf("Couldn't seek to address.\n");
  exit(1);
  }

fputc(value,sna);

fclose(sna);
  
exit(0);
}
