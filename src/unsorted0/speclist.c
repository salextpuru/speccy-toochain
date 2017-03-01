/* speclist by R. J. Marks 1994
 * Public domain
 *
 * 1998 somewhen :-) - changed to produce zmakebas-friendly output.
 */


#include <stdio.h>
#include <stdlib.h>

#define NAK "?"

unsigned char linebuf[32768]; /* big buffer for those lovely m/c REMs */
int progtotal;

/***** the spec-sci char set as transposed to normal ASCII *****/

/* nak = 'not a kharacter', of course! */
/* spacing may not end up being *exactly* the same, but is very close */

char *charset[]=
{
/* 000-009 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 010-019 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 020-029 */ NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,NAK,
/* 030-039 */ NAK,NAK," ","!","\"","#","$","%","&","'",
/* 040-049 */ "(",")","*","+",",","-",".","/","0","1",
/* 050-059 */ "2","3","4","5","6","7","8","9",":",";",
/* 060-069 */ "<","=",">","?","@","A","B","C","D","E",
/* 070-079 */ "F","G","H","I","J","K","L","M","N","O",
/* 080-089 */ "P","Q","R","S","T","U","V","W","X","Y",
/* 090-099 */ "Z","[","\\\\","]","^","_","`","a","b","c",
/* 100-109 */ "d","e","f","g","h","i","j","k","l","m",
/* 110-119 */ "n","o","p","q","r","s","t","u","v","w",
/* 120-129 */ "x","y","z","{","|","}","~","\\@","\\  ","\\ '",
/* 130-139 */ "\\' ", "\\''", "\\ .", "\\ :", "\\'.",
              "\\':", "\\. ", "\\.'", "\\: ", "\\:'",
/* 140-149 */ "\\..", "\\.:", "\\:.", "\\::", "\\a",
              "\\b","\\c","\\d","\\e","\\f",
/* 150-159 */ "\\g","\\h","\\i","\\j","\\k","\\l","\\m","\\n","\\o","\\p",
/* 160-169 */ "\\q","\\r","\\s"," SPECTRUM "," PLAY ",
              "RND","INKEY$","PI","FN ","POINT ",
/* 170-179 */ "SCREEN$ ","ATTR ","AT ","TAB ","VAL$ ",
              "CODE ","VAL ","LEN ","SIN ","COS ",
/* 180-189 */ "TAN ","ASN ","ACS ","ATN ","LN ",
              "EXP ","INT ","SQR ","SGN ","ABS ",
/* 190-199 */ "PEEK ","IN ","USR ","STR$ ","CHR$ ",
              "NOT ","BIN "," OR "," AND ","<=",
/* 200-209 */ ">=","<>"," LINE "," THEN"," TO ",
              " STEP "," DEF FN "," CAT "," FORMAT "," MOVE ",
/* 210-219 */ " ERASE "," OPEN#"," CLOSE#"," MERGE "," VERIFY ",
              " BEEP "," CIRCLE "," INK "," PAPER "," FLASH ",
/* 220-229 */ " BRIGHT "," INVERSE "," OVER "," OUT "," LPRINT ",
              " LLIST "," STOP "," READ "," DATA "," RESTORE ",
/* 230-239 */ " NEW "," BORDER "," CONTINUE "," DIM "," REM ",
              " FOR "," GOTO "," GOSUB "," INPUT "," LOAD ",
/* 240-249 */ " LIST "," LET "," PAUSE "," NEXT "," POKE ",
              " PRINT "," PLOT "," RUN "," SAVE "," RANDOMIZE",
/* 250-255 */ " IF "," CLS "," DRAW "," CLEAR "," RETURN"," COPY "
};




/* get a single speccy program line & line number & length */
void getline_1(FILE *in,int *linenum,int *linelen,int *t)
{
int b1,b2,f;

b1=fgetc(in);
b2=fgetc(in);
*linenum=b1*256+b2;
(*t)-=2;

b1=fgetc(in);
b2=fgetc(in);
*linelen=b1+256*b2;
(*t)-=2;

for(f=0;f<*linelen;f++)
  {
  linebuf[f]=fgetc(in);
  (*t)--;
  }
}


/* translate line into keywords using the charset array */
void xlatline(int linelen)
{
int f;

for(f=0;f<linelen-1;f++)
  {
  if((linebuf[0]!=234)&&(linebuf[f]==14))
    f+=5;  /* avoid inline FP numbers - but ok for REMs */
  else
    printf("%s",charset[linebuf[f]]);
  }
printf("\n");
}


/* process (opened) basic file to stdout */
void thrashfile(FILE *in)
{
int linelen,linenum,total;

total=progtotal;

getline_1(in,&linenum,&linelen,&total);
while(total>3)
  {
  printf("%4d",linenum);
  xlatline(linelen);
  getline_1(in,&linenum,&linelen,&total);
  }
printf("%4d",linenum);
xlatline(linelen);
}



int main(int argc,char *argv[])
{
FILE *in;

if(argc!=3)
  {
  printf("Speclist - PD by RJM for improbabledesigns.\n\n");
  printf("Lists ZX Spectrum raw basic program files to stdout.\n\n");
  printf("Usage:  speclist infile prog_offset >outfile.txt\n");
  exit(1);
  }

if((in=fopen(argv[1],"rb"))==NULL)
  fprintf(stderr,"Error: couldn't open file '%s'\n",argv[1]),exit(1);

progtotal=atoi(argv[2]);
thrashfile(in);      /* process it */
fclose(in);

exit(0);
}
