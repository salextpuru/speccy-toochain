#ifndef MHMT_OPTIMAL_H
#define MHMT_OPTIMAL_H
#include "mhmt-types.h"
#include "mhmt-globals.h"
#include "mhmt-lz.h"
// this structure exists in array for each input byte and used to build optimal code chain
struct optchain {
    struct lzcode code; // code of jumping here from previous position
    ULONG price; // bitprice (bitlength) of the best chain of lzcodes going up to this point:
    // initialized as 0xFFFFFFFF (maximum price)
    ULONG _just_a_filler_; // to make structure 8 bytes long (true for 32bit machines)
};
struct optchain * make_optch(ULONG actual_len);
void update_optch(ULONG position, struct lzcode * codes, ULONG (*get_lz_price)(ULONG position, struct lzcode * lzcode), struct optchain * optch);
void reverse_optch(struct optchain * optch, ULONG actual_len);
//struct lzcode * scan_optch(ULONG start_flag);
void free_optch(struct optchain * optch);
#endif
