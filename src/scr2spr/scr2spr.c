#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

#define SCR_W		32
#define SCR_H		24

#define	SCR_LEN		6912
#define	ATR_LEN		768

unsigned char	scr_data[SCR_LEN];
unsigned char	atr_data[ATR_LEN];

typedef struct spr{
	u_int16_t	size;
	u_int8_t	w;
	u_int8_t	h;
	u_int8_t	data[SCR_LEN+ATR_LEN];
};



// экран
int	x=0;
int	y=0;
int	w=SCR_W;
int	h=SCR_H;

// спрайт
struct spr sprite;

// Грузим экран
void load(const char* name){
	int fd=0;
	if(strcmp(name,"--")){
		fd = open(name, O_RDONLY);
	}

	if(fd < 0){
		fprintf(stderr,"Can't open file [%s]\n", name);
		exit(1);
	}

	read(fd,scr_data,SCR_LEN);
	read(fd,atr_data,ATR_LEN);

	close(fd);
}


// 
u_int16_t sadr(u_int16_t x, u_int16_t y){
	return(
		(x & 0x001F) |		// x
		((y << 5)  & 0x00E0) |	// y [0..2]
		((y << 8)  & 0x1800) 	// y [3..4]
	);
}

u_int16_t aadr(u_int16_t x, u_int16_t y){
	return(
		(x & 0x1F) |
		((y << 5) & 0x03E0) |
		0x1800
	);
}

u_int8_t* get_spr_line(u_int16_t x0, u_int16_t y0, u_int16_t w0,  u_int8_t* out){
	u_int16_t adr = sadr(x0,y0);

	u_int16_t il;
	u_int16_t ih;
	
	for(ih = 0; ih<8; ih++){
		for(il = 0; il<w0; il++){
			*(out++) = scr_data[ adr + (ih<<8) + il ];
		}
	}
	
	return(out);
}

u_int8_t*  get_spr_data(u_int8_t* out){
	u_int16_t ih;
	for(ih = y; ih<(y+h); ih++){
			out = get_spr_line(x,ih,w,out);
	}
	return(out);
}

u_int8_t*  get_spr_atr(u_int16_t x0, u_int16_t y0, u_int16_t w0,  u_int16_t h0, u_int8_t* out){
	u_int16_t adr = aadr(x0,y0);

	u_int16_t il;
	u_int16_t ih;
	
	for(ih = 0; ih<h0; ih++){
		for(il = 0; il<w0; il++){
			*(out++) = scr_data[ adr + (ih<<5) + il ];
		}
	}
	
	return(out);
}



void out_spr(){
	putc(sprite.size & 0xFF,stdout);
	putc((sprite.size>>8) & 0xFF,stdout);
	putc(sprite.w,stdout);
	putc(sprite.h,stdout);
	
	fwrite(sprite.data, sprite.size - 4 , 1, stdout);
}

int main ( int argc, char* argv[] ){
	if(argc < 6){
#if WITH_ATTRS
		fprintf(stderr,"Convert part of SCREEN$ to sprite (with attributs)\n"
			"USAGE:\n\tscr2spr <x> <y> <w> <h> <screen$ file> > sprite.bin");
#else
		fprintf(stderr,"Convert part of SCREEN$ to sprite (without attributs)\n"
			"USAGE:\n\tscr2spr <x> <y> <w> <h> <screen$ file> > sprite.bin");
#endif
		exit(1);
	}

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);

	load(argv[5]);

	sprite.w=w;
	sprite.h=h;

	get_spr_atr(x, y, w, h, get_spr_data(sprite.data) );

#if WITH_ATTRS
	sprite.size = w*h*8 + w*h + 4;
#else
	sprite.size = w*h*8 + 4;
#endif
	out_spr();

	return 0;
}
