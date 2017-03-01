#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MegaLZ_types.h"

#include "MegaLZ_globals.h"
#include "MegaLZ_pack.h"
#include "MegaLZ_twobyters.h"
#include "MegaLZ_hash.h"
#include "MegaLZ_emit.h"
#include "MegaLZ_lz.h"




struct packinfo * pdata; // array of packinfo elements for every byte of input file
                         // (dynamically allocated)







ULONG pack(void)
{
	// return value
	ULONG retcode=1;

	ULONG current_pos; // current position in file

	UBYTE curr_byte,last_byte; // current byte and last byte (for twobyter)

	struct lzcode curr, tmp;

	ULONG i,bitlen;

	ULONG skip;
	ULONG bestcode;
	LONG curgain,bestgain;

// allocate mem for arrays/tables/etc.

	if( stuff_init() )
	{
		// make hash
		make_hash();

		// very first inits
		pdata[0].best.disp=0;
		pdata[0].best.length=1; // 1st byte copied 'as-is'

		curr_byte=indata[0];


		if( mode==MODE_PACK_OPTIMAL )
		{ // !!optimal coding!!

			// initialize prices
			pdata[0].price=0;
			pdata[1].price=8;
			for(i=2;i<=inlen;i++)
			{
				pdata[i].price=0xffffffff;
			}

			// do every byte
			for( current_pos=1;current_pos<inlen;current_pos++)
			{
				last_byte=curr_byte;
				curr_byte=indata[current_pos];

				// add current twobyter
				if( !add_twobyter(last_byte,curr_byte,current_pos) )
				{
					printf("pack(): add_twobyter() failed!\n");
					retcode=0;
					goto ERROR;
				}

				// find all LZ codes for current byte
				make_LZ_codes(current_pos);

				// update prices
				i=0;
				do
				{
					bitlen=make_lz_info(0x00,&codes[i],NULL);
					if( !bitlen )
					{
						printf("pack(): make_lz_info() failed!\n");
						retcode=0;
						goto ERROR;
					}
					update_price(current_pos,bitlen,&codes[i]);
				} while( codes[i++].disp );
			}

			// reverse optimal chain
		        i=inlen;
		        tmp=pdata[i].best;
		        while(i>1)
		        {
				curr=tmp;
				i=i-curr.length;
				tmp=pdata[i].best;
				pdata[i].best=curr;
			}
		}
		else if( mode==MODE_PACK_GREEDY )
		{ // greedy coding

			// init chain
			for(i=0;i<inlen;i++)
			{
				pdata[i].best.disp=0;
				pdata[i].best.length=1;
			}

			skip=0;
			for( current_pos=1;current_pos<inlen;current_pos++)
			{
				last_byte=curr_byte;
				curr_byte=indata[current_pos];

				// add current twobyter
				if( !add_twobyter(last_byte,curr_byte,current_pos) )
				{
					printf("pack(): add_twobyter() failed!\n");
					retcode=0;
					goto ERROR;
				}

				if( skip )
				{
					skip--;
				}
				else
				{
					// find all LZ codes for current byte
					make_LZ_codes(current_pos);

					// find 'best' code on greedy basis
					bestgain=-2;
					bestcode=i=0;
					do
					{
						bitlen=make_lz_info(0x00,&codes[i],NULL);
						if( !bitlen )
						{
							printf("pack(): make_lz_info() failed!\n");
							retcode=0;
							goto ERROR;
						}

						curgain=(LONG)(8*codes[i].length)-bitlen;

						if( curgain>bestgain )
						{
							bestgain=curgain;
							bestcode=i;
						}
					} while( codes[i++].disp );

					// write found code, skip next (length-1) bytes
					pdata[current_pos].best=codes[bestcode];
					skip=codes[bestcode].length-1;
				}
			}
		}


		// generate output file
		if( !gen_output() )
		{
			printf("pack(): gen_output() failed!\n");
			retcode=0;
		}

	}
	else
	{
		printf("pack(): stuff_init() failed!\n");
		retcode=0;
	}

ERROR:
	stuff_free();

	return retcode;
}






ULONG stuff_init(void)
{
	//allocate/init different things for packing

	ULONG success;


	success=1;


	init_twobyters();

	if( !init_hash() ) success=0;



	// allocate packinfo data for input file
	pdata=(struct packinfo *)malloc( (inlen+1)*sizeof(struct packinfo) );
	if( !pdata )
	{
		success=0;
	}




	return success;
}



void stuff_free(void)
{
	//free different things used during packing




	free_twobyters();

	free_hash();


	if( pdata )
	{
		free( pdata );
	}
}







void make_LZ_codes(ULONG curpos)
{ // actual generation of all LZ codes for the given byte @curpos
  // result in codes[256] array (see MegaLZ_lz.h, MegaLZ_lz.c)

	UBYTE curbyte,nextbyte;
	ULONG i;
	ULONG tbi;
	ULONG was_twobyter,last_match;
	ULONG lzlen;

	struct tb_chain * curtb;



	tbi=0;
	curtb=NULL;

	// init lz codes collecting
	start_lz();


	// find 1-byte code

	curbyte=indata[curpos];

	i=(curpos>8)?(curpos-8):0;
	while( i<curpos )
	{
		if( indata[i]==curbyte )
		{
			add_lz((LONG)i-(LONG)curpos,1);
			break;
		}

		i++;
	}



	// find 2-byte code

	was_twobyter=0;

	if( (inlen-1)>curpos ) // if we have enough bytes ahead
	{
		nextbyte=indata[curpos+1];
		tbi=(((ULONG)curbyte)<<8)+((ULONG)nextbyte);

		curtb=tb_entry[tbi]; // get twobyter

		if( curtb )
		{
			if( (curpos-curtb->pos)<=256 )
			{
				add_lz((LONG)curtb->pos-(LONG)curpos,2);
			}

			if( (curpos-curtb->pos)<=4352 )
			{
				was_twobyter=1; // there is something for 3 and more bytes finding
			}
			else
			{
				cutoff_twobyte_chain(tbi,curpos); // flush older twobyters
			}
		}
	}



	// find other codes (len=3..255)

	if( was_twobyter && ((inlen-2)>curpos) ) // was twobyter within -1..-4352 range
	                                         // and at least 3-byter possible
	{
		// some inits

		//curtb=tb_entry[tbi]; // already done in 2-byter search

		last_match=1; // was last match (for 2 bytes)

		// loop for all possible lengths
		for( lzlen=3; (lzlen<=255)&&((inlen-lzlen+1)>curpos);/*NOTHING!*/)
		{
			if( last_match )
			{ // there were last match (lzlen-1)

				// compare last bytes of current match
				if( indata[curpos+lzlen-1]==indata[curtb->pos+lzlen-1] )
				{ // current match OK
					add_lz((LONG)curtb->pos-(LONG)curpos,lzlen);
					lzlen++;
				}
				else
				{ // current match failed: - take next (older) twobyter
MATCH_FAILED:
					curtb=curtb->next;
					if( !curtb ) break; // stop search if no more twobyters
					if( (curpos-curtb->pos)>4352 )
					{ // if twobyters out of range
						cutoff_twobyte_chain(tbi,curpos);
						break;
					}
					last_match=0; // indicate match fail for next step
				}
			}
			else
			{ // last match failed so compare all strings

				// first compare hashes of the ends of both strings
				if( hash[curpos+lzlen-1]==hash[curtb->pos+lzlen-1] )
				{
					// if hashes are the same, compare complete strings
					if( !memcmp( &indata[curpos], &indata[curtb->pos], lzlen) )
					{ //match OK
						last_match=1;
						add_lz((LONG)curtb->pos-(LONG)curpos,lzlen);
						lzlen++;
					}
					else goto MATCH_FAILED;
				}
				else goto MATCH_FAILED;
			}
		}
	}

	// stop collecting codes
	end_lz();

}










ULONG gen_output(void)
{
	ULONG pos;

	struct lzinfo lz;




	if( !emit_init() )
	{
		printf("gen_output(): emit_init() failed!\n");
		return 0;
	}


	pos=1; // first byte copied to output 'as-is', without LZ coding

	while( pos<inlen )
	{
		// write best code to the output stream
		if( !make_lz_info(indata[pos],&pdata[pos].best,&lz) )
		{
				printf("gen_output(): make_lz_info() failed!\n");
				return 0;
		}

		if( !emit_code(&lz) )
		{
			printf("gen_output(): emit_code() failed!\n");
			return 0;
		}
		pos+=lz.length; // step to the next unpacked byte
	}

	// sanity check
	if( pos>inlen )
	{
		printf("gen_output(): last position is out of input file!\n");
		return 0;
	}





	// just finish all deals
	if( !emit_finish() )
	{
		printf("gen_output(): emit_finish() failed!\n");
		return 0;
	}

	return 1;
}




void update_price(ULONG curpos,ULONG bitsize,struct lzcode * lz)
{ // update price for given data

	ULONG curprice;
	ULONG newpos;


	if( lz )
	{
		newpos=curpos+lz->length;
	}
	else
	{
		newpos=curpos+1;
	}

	curprice=pdata[newpos].price;

	if( curprice > bitsize+pdata[curpos].price )
	{
		pdata[newpos].price=bitsize+pdata[curpos].price;

		if(lz)
		{
			pdata[newpos].best= *lz;
		}
		else
		{
			pdata[newpos].best.length=1;
			pdata[newpos].best.disp=0;
		}
	}
}

