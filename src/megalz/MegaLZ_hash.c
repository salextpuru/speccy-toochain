#include <stdlib.h>

#include "MegaLZ_hash.h"

// hashes for every 3byte string in file: hash[i] is hash for indata[i-2],indata[i-1] and indata[i]
UBYTE * hash; // hash algorithm: (byte[0]>>>2) ^ (byte[1]>>>1) ^ byte[2]


ULONG init_hash(void)
{
	hash=(UBYTE *)malloc( inlen*sizeof(UBYTE) );

	return (hash!=NULL)?1:0;
}

void free_hash(void)
{
	if( hash ) free(hash);
}

void make_hash(void)
{ // makes hash for every byte of input file
  // for position i, hash is from bytes at [i-2],[i-1],[i]
  // first and second bytes does not have valid hash values

	UBYTE curr,prev,prev2;
	ULONG i;

	prev=curr=0;

	for(i=0;i<inlen;i++)
	{
		prev2=(UBYTE)((prev>>1)|(prev<<7));
		prev=(UBYTE)((curr>>1)|(curr<<7));
		curr=indata[i];

		hash[i]=(UBYTE)(prev2^prev^curr);
	}
}

