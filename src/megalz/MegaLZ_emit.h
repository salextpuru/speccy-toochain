#ifndef MEGALZ_EMIT_H
#define MEGALZ_EMIT_H

#include "MegaLZ_types.h"
#include "MegaLZ_lz.h"

// output buffer size
#define OUBUFSIZE 256


ULONG emit_init(void);
ULONG emit_code(struct lzinfo *);
ULONG emit_finish(void);

void emit_bits(ULONG,ULONG);



#endif

