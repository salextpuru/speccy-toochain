#include <stdio.h>
#include <unistd.h>
#include <string.h>


main(argc,argv)
int argc;
char **argv;
{
FILE *sna,*new;
int addr,c;

if(argc!=4)
  {
  printf("usage: ldirsna snapshot.sna insert_file addr_to_insert_at\n");
  exit(1);
  }

if((sna=fopen(argv[1],"r+"))==NULL)
  {
  printf("Couldn't open snapshot file.\n");
  exit(1);
  }

if((new=fopen(argv[2],"rb"))==NULL)
  {
  printf("Couldn't open file to insert.\n");
  exit(1);
  }

if(!strncmp(argv[3],"0x",2))
  sscanf(argv[3]+2,"%x",&addr);
else
  addr=atoi(argv[3]);

if(fseek(sna,addr-0x4000+27,SEEK_SET)==-1)
  {
  printf("Couldn't seek to address 0x%0.4X.\n",addr);
  exit(1);
  }

while((c=fgetc(new))!=EOF)
  {
  fputc(c,sna);
  }
  
fclose(sna);
fclose(new);

exit(0);
}
