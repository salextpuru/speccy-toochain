#ifndef MEGALZ_TWOBYTERS_H
#define MEGALZ_TWOBYTERS_H


#include "MegaLZ_types.h"


extern struct tb_chain * tb_entry[];
//extern struct tb_chain * tb_free;
//extern struct tb_bunch * tb_bunches;


// size of bunches of twobyters
#define BUNCHSIZE 256



//chained two-byter element
struct tb_chain
{
	struct tb_chain * next; // previous entry of the same 2-byte or NULL if none

	ULONG pos; // position where 2-byte last seen - max 4352 bytes back from current!
};

//container of bunch of twobyters
struct tb_bunch
{
	struct tb_bunch * next;

	struct tb_chain bunch[BUNCHSIZE];
};


void init_twobyters(void);
void free_twobyters(void);

ULONG add_twobyter(UBYTE,UBYTE,ULONG);
void cutoff_twobyte_chain(ULONG,ULONG);
ULONG add_bunch_of_twobyters(void);
struct tb_chain * get_free_twobyter(void);







#endif

