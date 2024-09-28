
#include "block.h"

struct FallingBlock {
	int x;
	int y;
	BlockType type;
	RotateState rotate;
};

static const char *blocks_desciption[BLOCK_COUNT] = {/*{{{*/
	"    "
	"@@@@",

	" @@ "
	" @@ ",

	" @  "
	"@@@ ",

	" @@ "
	"@@  ",

	"@@  "
	" @@ ",

	"@   "
	"@@@ ",

	"  @ "
	"@@@ ",
};/*}}}*/

// for analyser program
int block_color[BLOCK_COUNT][3] = {
	{0, 127, 255},
	{255, 255, 0},
	{127, 0, 255},
	{0, 255, 0},
	{255, 0, 0},
	{0, 0, 255},
	{255, 127, 0},
};



