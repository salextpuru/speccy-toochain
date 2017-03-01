/* tzxplay - play a tzx file on /dev/dsp.
 * incomplete, but good enough for MD2. :-)
 * compile with inline functions if poss, some here are very short!
 */

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>


/* data blocks are read in to here when needed */
unsigned char data_buf[65536];


/* converts t-states to samples. */
#define TSTATES_TO_SAMPLES(x)	(((x)*441)/35000)

/* 1ms in samples; actually 44.1 but this'll do. */
#define ONE_MS_TIME	44

/* sample values output for `high' and `low' */
#define SAMPLE_HIGH	0xff
#define SAMPLE_LOW	0x00

#if 0
#define SAMPLE_LOW	0x02
#define SAMPLE_HIGH	0x80
#endif

/* the current value, inverted *after* each pulse (half-period) */
int current_val=SAMPLE_LOW;	/* initial state required in tzx files */

FILE *output_file;


/* output a tone at current level for N t-states. */
void tone(int tstates)
{
int f,samples=TSTATES_TO_SAMPLES(tstates);

for(f=0;f<samples;f++) fputc(current_val,output_file);
}


/* flip the state of the current level. */
void flip_level()
{
current_val=((current_val==SAMPLE_HIGH)?SAMPLE_LOW:SAMPLE_HIGH);
}


/* do a pulse, i.e. output at current level then flip it ready for next one. */
void pulse(int tstates)
{
flip_level();
tone(tstates);
}


/* do a pause of N milliseconds, defined thus:
 "A 'Pause' block consists of a  'low' pulse level of some duration.  To ensure
  that the last edge produced is properly finished there should  be atleast 1ms
  pause  of the  *opposite* level and  only after that  the pulse should go  to
  'low'. At the end of a 'Pause' block the 'current pulse level' is low.  (Note
  that the first pulse will therefore not immediately produce an edge). A pause
  block of  zero duration is completely ignored, so the 'current  pulse  level'
  will *not* change in this case.  This also applies to 'Data' blocks that have
  some pause duration included in them."
*
* NB: this isn't called simply `pause' since this name is used by a Unix
* syscall.
*/
void do_pause(int ms)
{
int f;

if(ms==0) return;

/* 1ms pause of opposite level */
flip_level();
for(f=0;f<ONE_MS_TIME+1;f++)	/* plus one to allow for rounding error */
  fputc(current_val,output_file);

current_val=SAMPLE_LOW;
for(f=0;f<ms*ONE_MS_TIME;f++)
  fputc(current_val,output_file);
}


/* generate a `pilot tone' (personally I always called it the leader tone,
 * but maybe that was just me), **including the sync bits**.
 * len_of_pulse is in t-states, tonelen_in_pulses is, er, in pulses. :-)
 * sync1 is first sync pulse time in t-states, sync2 the second.
 */
void pilot_tone(int len_of_pulse,int tonelen_in_pulses,int sync1,int sync2)
{
int f;

for(f=0;f<tonelen_in_pulses;f++)
  pulse(len_of_pulse);

pulse(sync1);
pulse(sync2);
}


/* send a single data bit.
 * lengths in t-states, bitval is zero if zero bit else non-zero.
 */
void send_bit(int zero_pulselen,int one_pulselen,int bitval)
{
int len=(bitval?one_pulselen:zero_pulselen);

pulse(len);	/* low */
pulse(len);	/* high */
}


/* send raw data bits *without pilot tone*.
 * data is data to send, len is length in bytes, last_byte_used_bits
 * is no. used bits in last byte (usually 8, see tzx spec for block ID
 * 11); see send_bit() for other arg meanings.
 */
void send_raw_data(unsigned char *data,int len,int last_byte_used_bits,
	int zero_pulselen,int one_pulselen)
{
int f,i,val,mask,tmp;

if(len==0) return;

for(f=0;f<len-1;f++)	/* last byte dealt with specially */
  {
  mask=128;
  val=data[f];
  for(i=0;i<8;i++,mask>>=1)
    send_bit(zero_pulselen,one_pulselen,val&mask);
  }

/* now do last byte */
mask=128;
val=data[len-1];
for(i=0;i<8;i++,mask>>=1)
  {
  tmp=(val&mask);
  if(i>=last_byte_used_bits) tmp=0;
  send_bit(zero_pulselen,one_pulselen,tmp);
  }
}


/* get little-endian 16-bit word from file. */
int get_word(FILE *in)
{
int tmp=fgetc(in);
return(tmp+256*fgetc(in));
}


/* parse and send normal block. */
void normal_block(FILE *in)
{
int pauselen,datalen;

pauselen=get_word(in);
datalen=get_word(in);
fread(data_buf,1,datalen,in);

/* following values (these really can't change, as they're burned into 2^N
 * speccy ROMs :-)) from tzx spec.
 */
/* amazingly, there is ABSOLUTELY NO WAY to tell the difference between
 * a header block and data block in the tzx spec. So here we assume
 * that a block 19 bytes long (17 plus the xor `checksum' stuff) is
 * a header, and use the longer tone. Bogus, but it's all I can do,
 * and at least it shouldn't actually break anything for a 17-byte file.
 */
pilot_tone(2168,(datalen==19)?8064:3220,667,735);
send_raw_data(data_buf,datalen,8,855,1710);
do_pause(pauselen);
}


/* parse and send turboload block. */
void turbo_block(FILE *in)
{
int pauselen,datalen;
int pilot_pulse,sync1,sync2,zero,one,pilot_tonelen,bits_in_last;

pilot_pulse=get_word(in);
sync1=get_word(in);
sync2=get_word(in);
zero=get_word(in);
one=get_word(in);
pilot_tonelen=get_word(in);
bits_in_last=fgetc(in);
pauselen=get_word(in);
datalen=get_word(in);
datalen+=65536*fgetc(in);
fread(data_buf,1,datalen,in);

pilot_tone(pilot_pulse,pilot_tonelen,sync1,sync2);
send_raw_data(data_buf,datalen,bits_in_last,zero,one);
do_pause(pauselen);
}


void pure_tone(FILE *in)
{
int f;
int tonelen_in_pulses,len_of_pulse;

len_of_pulse=get_word(in);
tonelen_in_pulses=get_word(in);

for(f=0;f<tonelen_in_pulses;f++)
  pulse(len_of_pulse);
}


void multiple_pure_tone(FILE *in)
{
int f,g;
int tonelen_in_pulses,len_of_pulse;
int num_tones;

num_tones=fgetc(in);

for(f=0;f<num_tones;f++)
  pulse(get_word(in));
}


void pure_data(FILE *in)
{
int zerolen,onelen,last_byte_used_bits;
int pauselen,datalen;

zerolen=get_word(in);
onelen=get_word(in);
last_byte_used_bits=fgetc(in);
pauselen=get_word(in);

datalen=get_word(in);
/* actually it *could* handle an exactly-64k block, but this is less
 * hassle. :-)
 */
if(fgetc(in)>0) fprintf(stderr,"can't handle >=64k block!\n"),exit(1);
fread(data_buf,1,datalen,in);

send_raw_data(data_buf,datalen,last_byte_used_bits,zerolen,onelen);
do_pause(pauselen);
}



int main()
{
int c,f,id,spd;

if((output_file=fopen("/dev/dsp","wb"))==NULL)
  fprintf(stderr,"couldn't open /dev/dsp\n"),exit(1);

spd=44100;
if((ioctl(fileno(output_file),SNDCTL_DSP_SPEED,&spd))==-1)
  fprintf(stderr,"Error setting frequency; carrying on anyway...\n");

fprintf(stderr,"spd %d\n",spd);

do_pause(1000);


/* XXX should test magic, version, etc. */
for(f=0;f<10;f++) getchar();

while((id=getchar())!=EOF)
  {
printf("ID=%02X at %d\n",id,ftell(stdin));
/*  printf("block ID %02Xh\n",id);*/
  
  switch(id)
    {
    case 0x10:	/* normal */
      normal_block(stdin); break;

    case 0x11:	/* turbo */
      turbo_block(stdin); break;

    case 0x12:	/* pure tone */
      pure_tone(stdin); break;

    case 0x13:	/* pure tones */
      multiple_pure_tone(stdin); break;

    case 0x14:	/* pure data */
      pure_data(stdin); break;
    
    case 0x20:	/* pause */
      c=getchar();
      c+=256*getchar();
      if(c!=0)
        do_pause(c);
      else
        {
        /* XXX bleah :-) */
        static char buf[256];
        FILE *in=fopen("/dev/tty","r");
        do_pause(1000);		/* to avoid clicky probs */
        printf("-- tape stopped on pause block, press enter to play -- ");
        fflush(stdout);
        fgets(buf,sizeof(buf),in);
        fclose(in);
        do_pause(1000);		/* to avoid clicky probs */
        }
      break;
    
    case 0x21:	/* group start - ignored */
      c=getchar();
      for(f=0;f<c;f++) getchar();
      break;
      
    case 0x22:	/* group end - ignored */
      /* no body */
      break;
    
    case 0x30:	/* message */
      printf("msg block: ");
      c=getchar();
      for(f=0;f<c;f++) putchar(getchar());
      putchar('\n');
      break;
    
    default:
      fprintf(stderr,"unhandled block type (ID=%02Xh), aborting!\n",id);
      exit(1);
    }
  }

do_pause(500);	/* make sure there's no click to ruin it! */

exit(0);
}
