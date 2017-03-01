
#include "stdafx.h"
#include "defs.h"
#include "msg.h"
void print_help()
{
	printf(STR(SB_VER));
	printf("\n\n");
	printf("Build:	  spgbld -b <input.ini> <output.spg> [-c <pack method>]\n");
	printf("Unpack:	  spgbld -u <input.spg>\n");
	printf("Pack method is:	 auto/-1(default) none/0 mlz/1 hst/2\n");
	printf("\n\n");
}
