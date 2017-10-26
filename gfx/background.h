/*
 	BACKGROUND.H
	A tilemapped background. 
*/

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "jagcore.h"
#include "mobj.h"
#include "shared.h"

#define BACKGROUND_TILES_WIDE	16
#define BACKGROUND_TILES_TALL	13
#define BACKGROUND_TILES_COUNT	BACKGROUND_TILES_WIDE*BACKGROUND_TILES_TALL

extern op_branch_object branch_background_row[BACKGROUND_TILES_TALL+1];
extern MotionObject mobj_bg_tiles[BACKGROUND_TILES_COUNT];

void BG_Setup(MotionObject link);

#endif
