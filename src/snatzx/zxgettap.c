/* 44kHz version of zxget - seems more reliable now, got
 * 16k across (the rom) perfectly first time!
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>


/* x is cycle time, 54.6 would be perfect */
#define IS_GOOD_TONE(x) (((x)>30)&&((x)<90))
#define TIMEOUT(x)      ((x)==-1)


unsigned char databuf[65536];  /* as much as you could possibly save */

int checkbyte;



int getaudio(audio)
int audio;
{
unsigned char c;

read(audio,&c,1);
if(c>0xA0) return(1);
return(0);
}


/* this times the 1 -> 0, then 0 -> 1 cycle and returns the length in
 * samples. If the input isn't 1 to begin with, it waits until it is.
 */
int timeswitch(audio)
int audio;
{
int time_taken,timeout;

timeout=12000;
while((getaudio(audio)!=1)&&(timeout!=0)) timeout--;
if(timeout==0)
  return(-1);

timeout=12000;
time_taken=1;

do
  {
  time_taken++;
  timeout--;
  }
while((getaudio(audio)==1)&&(timeout!=0));

do
  {
  time_taken++;
  timeout--;
  }
while((getaudio(audio)==0)&&(timeout!=0));

if(timeout==0)
  return(-1);
else
  return(time_taken);
}


int findtone(int audio)
{
int t;

do
  t=timeswitch(audio);
while((!IS_GOOD_TONE(t))||(TIMEOUT(t)));

fprintf(stderr,"<loading block>\n");

do
  t=timeswitch(audio);
while((IS_GOOD_TONE(t))&&(!TIMEOUT(t)));

/* well, it's either slipped to the start of the stuff, or that was junk.
 * it's got to be a 1 or 0.
 */
if(t>=54 || TIMEOUT(t))
  return(0);

return(1);
}


int getbyte(int audio)
{
int t,f,dat;

dat=0;
for(f=0;f<8;f++)
  {
  dat<<=1;
  t=timeswitch(audio);
  /* zero is 21.55, one is 42.83 */
  if(t>29 && t<59) dat|=1;
  if(t<12 || t>=59)
    return(EOF);
  }

checkbyte^=dat;
return(dat);
}


int main()
{
int audio,length;
int spd,c,prevchk2,prevchk,initchk;

audio=open("/dev/dsp",O_RDONLY,0);
if(audio==-1)
  fprintf(stderr,"Couldn't open /dev/dsp.\n"),exit(1);

spd=44100;
if(ioctl(audio,SNDCTL_DSP_SPEED,&spd)==-1)
  fprintf(stderr,"Error setting frequency, carrying on anyway");


while(1)
  {
  /* get lead-in tone */
  if(!findtone(audio))
    continue;
  
  if((initchk=getbyte(audio))==-1)
    {
    fprintf(stderr,"error: bad init. val. for check byte\n");
    continue;
    }
  
  checkbyte=initchk;
  length=0;
  prevchk=prevchk2=-1;
  while((c=getbyte(audio))!=EOF)
    {
    databuf[length++]=c;
    prevchk2=prevchk;
    prevchk=checkbyte;
    
    if(length%4096==0 && length>0)
      fprintf(stderr,"<%dk...>\n",length/1024);
    }
  
  /* this is length plus check byte currently, so if zero
   * it screwed up.
   */
  if(length==0)
    {
    fprintf(stderr,"error: no check byte (loading error)\n");
    continue;
    }
  
  /* the last byte read is really the check byte */
  length--;
  if(databuf[length]!=prevchk2)
    {
    fprintf(stderr,"error: check bytes don't match (loading error)\n");
    continue;
    }
  
  fprintf(stderr,"<%d byte block read ok>\n",length);
  
  /* output the block */
  putchar((length+2)%256);
  putchar((length+2)/256);
  putchar(initchk);
  fwrite(databuf,1,length,stdout);
  putchar(prevchk2);
  fflush(stdout);
  }

/* not reached, but FWIW :-) */
close(audio);
return(0);
}


