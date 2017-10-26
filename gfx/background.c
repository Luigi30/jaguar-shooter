#include "background.h"

MotionObject mobj_bg_tiles[BACKGROUND_TILES_COUNT];
op_branch_object branch_background_row[BACKGROUND_TILES_TALL+1];

void BG_Setup(MotionObject link)
{
	//Create the bitmap objects.
	//Link = the object that the end of a row links to.

	for(int i=BACKGROUND_TILES_COUNT-1; i>=0; i--)
	{
		mobj_bg_tiles[i].graphic 	= calloc(1,sizeof(op_bmp_object));
		mobj_bg_tiles[i].objType 	= BITOBJ;
		mobj_bg_tiles[i].position.x = 19 + ((i % BACKGROUND_TILES_WIDE) * 16);
		mobj_bg_tiles[i].position.y = 80 + ((i / BACKGROUND_TILES_WIDE) * 32);	// 2 Ys = 1 px
		mobj_bg_tiles[i].pxWidth  	= 16;
		mobj_bg_tiles[i].pxHeight 	= 16;
		
		mobj_bg_tiles[i].animations = NULL;
		
		mobj_bg_tiles[i].graphic->p0.type	= mobj_bg_tiles[i].objType;	       	/* BITOBJ = bitmap object */
		mobj_bg_tiles[i].graphic->p0.ypos	= mobj_bg_tiles[i].position.y;      /* YPOS = Y position on screen "in half-lines" */
		mobj_bg_tiles[i].graphic->p0.height = mobj_bg_tiles[i].pxHeight;	        /* in pixels */

		if(i % BACKGROUND_TILES_WIDE != BACKGROUND_TILES_WIDE-1) {
			mobj_bg_tiles[i].graphic->p0.link	= (uint32_t)mobj_bg_tiles[i+1].graphic >> 3;	/* link to next object */
		} else {
			//Last one of a row - object links to link
			mobj_bg_tiles[i].graphic->p0.link	= (uint32_t)link.graphic >> 3;	/* link to next object */
		}
		
		mobj_bg_tiles[i].graphic->p0.data	= (uint32_t)shipsheet >> 3;	/* ptr to pixel data */
		mobj_bg_tiles[i].graphic->p1.xpos	= mobj_bg_tiles[i].position.x;      /* X position on screen, -2048 to 2047 */
		mobj_bg_tiles[i].graphic->p1.depth	= O_DEPTH8 >> 12;		/* pixel depth of object */
		mobj_bg_tiles[i].graphic->p1.pitch	= 1;				/* 8 * PITCH is added to each fetch */
		mobj_bg_tiles[i].graphic->p1.dwidth = 256 / 8;	/* pixel data width in 8-byte phrases */
		mobj_bg_tiles[i].graphic->p1.iwidth = mobj_bg_tiles[i].pxWidth / 8;	/* image width in 8-byte phrases, for clipping */	
		mobj_bg_tiles[i].graphic->p1.release= 0;				/* bus mastering, set to 1 when low-depth */
		mobj_bg_tiles[i].graphic->p1.trans  = 1;				/* makes color 0 transparent */
		mobj_bg_tiles[i].graphic->p1.index  = 0;
	}

	//Create the branch objects.
	for(int i=0; i<BACKGROUND_TILES_TALL; i++)
	{
		branch_background_row[i].type = BRANCHOBJ;
		branch_background_row[i].link = (uint32_t)mobj_bg_tiles[(BACKGROUND_TILES_TALL-i-1) * BACKGROUND_TILES_WIDE].graphic >> 3;
		branch_background_row[i].cc = BRANCH_CC_GT;
		branch_background_row[i].ypos = 80 + (BACKGROUND_TILES_TALL-1 *32) + ((BACKGROUND_TILES_TALL-i) * 32);
	}

	branch_background_row[BACKGROUND_TILES_TALL].type = BRANCHOBJ;
	branch_background_row[BACKGROUND_TILES_TALL].cc = BRANCH_CC_LT;
	branch_background_row[BACKGROUND_TILES_TALL].ypos = 2047;
	branch_background_row[BACKGROUND_TILES_TALL].link = (uint32_t)stopobj >> 3;
	
}