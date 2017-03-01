#include <stdlib.h>

#include "MegaLZ_twobyters.h"


// two-byte lookup entry table
struct tb_chain * tb_entry[65536]; //array of ptrs to tb_chain chains or NULL if match not exists
                                   // array index i=FirstByte*256+SecondByte (65536 entries total)
                                   // at every position in input file current tb_entry points to the
                                   // displacements from current pos: -1 .. -4352
struct tb_chain * tb_free; // linked list of freed tb_chain entries - NULL at the beginning

struct tb_bunch * tb_bunches; // all allocated bunches as linked list




//----------------------------------------------------------------

void init_twobyters(void)
{ // init pointers tb_free, tb_bunches and tb_entry array

	ULONG i;


	tb_free=NULL; // init linked list of free tb_chain elements

	tb_bunches=NULL; // no bunches already allocated

	for(i=0;i<0x10000;i++) // init array of 2-byte match pointers
	{
		tb_entry[i]=NULL;
	}
}

//----------------------------------------------------------------

void free_twobyters(void)
{ // free all twobyter-related memory

	struct tb_bunch * tbtmp;

	while( tb_bunches )
	{
		tbtmp=tb_bunches;
		tb_bunches=tb_bunches->next;
		free( tbtmp );
	}
}

//----------------------------------------------------------------

ULONG add_twobyter(UBYTE last,UBYTE curr,ULONG curpos)
{ // adds new twobyter to the array of chains

	struct tb_chain * newtb;
	ULONG index;


	index=(((ULONG)last)<<8)+((ULONG)curr);

	newtb=get_free_twobyter();
	if( !newtb )
	{ // no free elements

		// first try to flush current chain
		cutoff_twobyte_chain(index,curpos);

		newtb=get_free_twobyter();
		if( !newtb )
		{ // nothing free - allocate new bunch
			if( !add_bunch_of_twobyters() )
			{
				return 0;
			}

			newtb=get_free_twobyter();
		}
	}



	newtb->next=tb_entry[index];
	tb_entry[index]=newtb;

	newtb->pos=curpos-1; // points to the first byte of given two bytes

	return 1;
}

//----------------------------------------------------------------

void cutoff_twobyte_chain(ULONG index,ULONG curpos)
{ // cuts off given chain so that there are no older than 4352 bytes back elements

	struct tb_chain * curr, * prev;


	curr=tb_entry[index];
	if( !curr ) return;

	// see if we should delete elements after first element in the given chain
	prev=curr;
	curr=curr->next;

	while( curr )
	{
		if( (curpos-(curr->pos))>4352 )
		{
			prev->next=curr->next; // remove from chain

			curr->next=tb_free; // insert into free chain
			tb_free=curr;

			curr=prev->next; // step to the next element in chain
		}
		else
		{
			prev=curr;
			curr=curr->next;
		}
	}

	// delete first element in chain if needed
	curr=tb_entry[index];
	if( (curpos-(curr->pos))>4352 )
	{
		tb_entry[index]=curr->next;
		curr->next=tb_free;
		tb_free=curr;
	}
}

//----------------------------------------------------------------

ULONG add_bunch_of_twobyters(void)
{ // adds a bunch of twobyters to the free list

	ULONG i;
	struct tb_bunch * newbunch;

	// alloc new bunch
	newbunch=(struct tb_bunch *)malloc( sizeof(struct tb_bunch) );
	if( newbunch==NULL ) return 0;

	// link every twobyter into one list
	for(i=0;i<(BUNCHSIZE-1);i++)
	{
		newbunch->bunch[i].next=&(newbunch->bunch[i+1]);
	}

	// add this list to the free list
	newbunch->bunch[BUNCHSIZE-1].next=tb_free;
	tb_free=&(newbunch->bunch[0]);

	// add bunch to bunches list
	newbunch->next=tb_bunches;
	tb_bunches=newbunch;

	return 1;
}

//----------------------------------------------------------------

struct tb_chain * get_free_twobyter(void)
{ // gets from tb_free new free twobyter or return NULL if no free twobyters
	struct tb_chain * newtb;

	if( tb_free )
	{
		newtb=tb_free;
		tb_free=tb_free->next;

		return newtb;
	}
	else
	{
		return NULL;
	}
}

