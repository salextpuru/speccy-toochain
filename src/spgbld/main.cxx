
#include "stdafx.h"
#include "defs.h"
#include "msg.h"
#include "func.h"
HDR hdr;
BLK blk[256];
CONF conf;
int main(int argc, char* argv[])
{
	init_hdr();
	parse_args(argc, argv);

	switch (conf.mode) {
	case M_BLD:
		load_ini(conf.in_fname);
		load_files();

		if (conf.packer != PM_NONE)
			pack_blocks();

		save_spg(conf.out_fname);
		break;

	case M_UNP:
		unpack_spg(conf.in_fname);
		break;
	}

	error(OK);
}
