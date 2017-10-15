/* Note: All malloc() objects or anything on the heap is phrase-aligned.
   Objects on the stack are word-aligned. */

#include "tyrian.h"
#include <stdbool.h>

extern uint32_t gpu_register_dump[32];

char skunkoutput[256];

op_stop_object *make_stopobj() {	
  op_stop_object *stopobj = calloc(1,sizeof(op_stop_object));
  stopobj->type = STOPOBJ;
  stopobj->int_flag = 1;
  return stopobj;
}

bool vbl_flag = false;

uint16_t jag_custom_interrupt_handler()
{
  if (*INT1&C_VIDENA)
    {      
      //The height field needs to be reset each frame for each mobj. Thanks Atari.
      mobj_background.graphic->p0.height = 200;
      mobj_font.graphic->p0.height = 200;
      mobj_sprites.graphic->p0.height = 200; 

      mobj_background.graphic->p0.data = (uint32_t)front_buffer >> 3;
      mobj_font.graphic->p0.data = (uint32_t)text_buffer >> 3;

      MMIO16(INT2) = 0;
      return C_VIDCLR;
    }
  return 0;
}

void clear_video_buffer(uint8_t *buffer){
  BLIT_rectangle_solid(buffer, 0, 0, 320, 200, 0);
}

void draw_status_bar(uint8_t *buffer)
{
  BLIT_rectangle_solid(buffer, 263, 0, 56, 200, 0x4949494949494949);

  BLIT_8x8_text_string(text_buffer, 280, 16, "S");
  BLIT_8x8_text_string(text_buffer, 280, 24, "T");
  BLIT_8x8_text_string(text_buffer, 280, 32, "A");
  BLIT_8x8_text_string(text_buffer, 280, 40, "T");
  BLIT_8x8_text_string(text_buffer, 280, 48, "U");
  BLIT_8x8_text_string(text_buffer, 280, 56, "S");
}

int main() {
  //set correct endianness
  MMIO32(G_END) = 0x00070007;
  
  srand(8675309);
  jag_console_hide();
  
  //BLIT_init_blitter();
  BLITTER_LOCK_CPU = false;
  BLITTER_LOCK_GPU = false;
  
  skunkCONSOLEWRITE("Connected to PC.\n");

  skunkCONSOLEWRITE("Uploading sprite program to GPU\n");
  GPU_LOAD_SPRITE_PROGRAM();
  
  front_buffer = background_frame_0;
  back_buffer = background_frame_1;

  PALETTES_initialize();
  PALETTES_load_from_array(shipsheet_rgb_pal);

  //Text layer color
  jag_set_indexed_color(254, toRgb16(0,0,0));
  jag_set_indexed_color(255, toRgb16(200, 200, 200));

  BLIT_8x8_text_string(text_buffer, 32, 16, "                   ");

  /* STOP object ends the object list */
  op_stop_object *stopobj = make_stopobj();

  /* Font bitmap thingy */
  {
    mobj_font.graphic = calloc(1,sizeof(op_bmp_object));
    mobj_font.objType = BITOBJ;
    mobj_font.position.x = 19;
    mobj_font.position.y = 80;
    mobj_font.pxWidth = 320;
    mobj_font.pxHeight = 200;
    
    mobj_font.animations = NULL;
    
    mobj_font.graphic->p0.type	= mobj_font.objType;	       	/* BITOBJ = bitmap object */
    mobj_font.graphic->p0.ypos	= mobj_font.position.y;         /* YPOS = Y position on screen "in half-lines" */
    mobj_font.graphic->p0.height = mobj_font.pxHeight;	        /* in pixels */
    mobj_font.graphic->p0.link	= (uint32_t)stopobj >> 3;	/* link to next object */
    mobj_font.graphic->p0.data	= (uint32_t)text_buffer >> 3;	/* ptr to pixel data */
    mobj_font.graphic->p1.xpos	= mobj_font.position.x;         /* X position on screen, -2048 to 2047 */
    mobj_font.graphic->p1.depth	= O_DEPTH1 >> 12;		/* pixel depth of object */
    mobj_font.graphic->p1.pitch	= 1;				/* 8 * PITCH is added to each fetch */
    mobj_font.graphic->p1.dwidth= mobj_font.pxWidth / 64;	/* pixel data width in 8-byte phrases */
    mobj_font.graphic->p1.iwidth= mobj_font.pxWidth / 64;	/* image width in 8-byte phrases, for clipping */	
    mobj_font.graphic->p1.release= 0;				/* bus mastering, set to 1 when low-depth */
    mobj_font.graphic->p1.trans  = 1;				/* makes color 0 transparent */
    mobj_font.graphic->p1.index  = 127;
  }

  skunkCONSOLEWRITE("font layer initialized\n");

  /* Sprite layer */
  {
    mobj_sprites.graphic = calloc(1,sizeof(op_bmp_object));
    mobj_sprites.objType = BITOBJ;
    mobj_sprites.position.x = 19;
    mobj_sprites.position.y = 80;
    mobj_sprites.pxWidth = 320;
    mobj_sprites.pxHeight = 200;
    
    mobj_sprites.animations = NULL;
    
    mobj_sprites.graphic->p0.type	= mobj_sprites.objType;	       	/* BITOBJ = bitmap object */
    mobj_sprites.graphic->p0.ypos	= mobj_sprites.position.y;      /* YPOS = Y position on screen "in half-lines" */
    mobj_sprites.graphic->p0.height     = mobj_sprites.pxHeight;	        /* in pixels */
    mobj_sprites.graphic->p0.link	= (uint32_t)mobj_font.graphic >> 3;	/* link to next object */
    mobj_sprites.graphic->p0.data	= (uint32_t)sprite_buffer >> 3;	/* ptr to pixel data */
    mobj_sprites.graphic->p1.xpos	= mobj_sprites.position.x;      /* X position on screen, -2048 to 2047 */
    mobj_sprites.graphic->p1.depth	= O_DEPTH8 >> 12;		/* pixel depth of object */
    mobj_sprites.graphic->p1.pitch	= 1;				/* 8 * PITCH is added to each fetch */
    mobj_sprites.graphic->p1.dwidth     = mobj_sprites.pxWidth / 8;	/* pixel data width in 8-byte phrases */
    mobj_sprites.graphic->p1.iwidth     = mobj_sprites.pxWidth / 8;	/* image width in 8-byte phrases, for clipping */	
    mobj_sprites.graphic->p1.release= 0;				/* bus mastering, set to 1 when low-depth */
    mobj_sprites.graphic->p1.trans  = 1;				/* makes color 0 transparent */
    mobj_sprites.graphic->p1.index  = 0;
  }

   /* Background */
  {
    mobj_background.graphic = calloc(1,sizeof(op_bmp_object));
    mobj_background.objType = BITOBJ;
    mobj_background.position.x = 19;
    mobj_background.position.y = 80;
    mobj_background.pxWidth = 320;
    mobj_background.pxHeight = 200;
    
    mobj_background.graphic->p0.type	= mobj_background.objType;	/* BITOBJ = bitmap object */
    mobj_background.graphic->p0.ypos	= mobj_background.position.y;   /* YPOS = Y position on screen "in half-lines" */
    mobj_background.graphic->p0.height  = mobj_background.pxHeight;	/* in pixels */
    mobj_background.graphic->p0.link	= (uint32_t)mobj_font.graphic >> 3;	/* link to next object */
    mobj_background.graphic->p0.data	= (uint32_t)front_buffer >> 3;	/* ptr to pixel data */
    
    mobj_background.graphic->p1.xpos	= mobj_background.position.x;      /* X position on screen, -2048 to 2047 */
    mobj_background.graphic->p1.depth	= O_DEPTH8 >> 12;		/* pixel depth of object */
    mobj_background.graphic->p1.pitch	= 1;				/* 8 * PITCH is added to each fetch */
    mobj_background.graphic->p1.dwidth  = mobj_background.pxWidth / 8;	/* pixel data width in 8-byte phrases */
    mobj_background.graphic->p1.iwidth  = mobj_background.pxWidth / 8;	/* image width in 8-byte phrases, for clipping */	
    mobj_background.graphic->p1.release= 0;				/* bus mastering, set to 1 when low-depth */
    mobj_background.graphic->p1.trans  = 1;				/* makes color 0 transparent */
    mobj_background.graphic->p1.index  = 0;
  }

  skunkCONSOLEWRITE("background layer initialized\n");
  
  //Start the list here.
  jag_attach_olp(mobj_background.graphic);

  skunkCONSOLEWRITE("object list attached\n");
		
  uint32_t stick0, stick0_lastread;
  uint16_t framecounter = 0;
  uint32_t framenumber = 0;

  int current_palette = 0;
  char palette_string[32];

  Coordinate player_ship_coords = (Coordinate){ .x = 100, .y = 140 };
  int player_side_movement_frames = 0;

  list_Sprites = malloc(sizeof(struct List));
  NewList(list_Sprites);

  list_Bullets = malloc(sizeof(struct List));
  NewList(list_Bullets);

  for(int j=0;j<5;j++){
    for(int i=0;i<10;i++){
      SpriteNode *node = SpriteNode_Create((Coordinate){ .x = 24*i, .y = 25*j },
					   SPRITES_find("NME_GrayJet"),
					   (Coordinate){ .x = 0, .y = 0 },
					   false);
      AddHead(list_Sprites, (struct Node *)node);
    }
  }

  SpriteNode *player = SpriteNode_Create(player_ship_coords,
					 SPRITES_find("USPTalon"),
					 (Coordinate){ .x = 0, .y = 0 },
					 true);
  player->node.ln_Name = "PLAYER_SHIP";
  AddHead(list_Sprites, player);

  sprintf(skunkoutput, "head: %p | pred %p | succ %p\n", list_Sprites->lh_Head, list_Sprites->lh_Head->ln_Pred, list_Sprites->lh_Head->ln_Succ);
  skunkCONSOLEWRITE(skunkoutput);

  //SpriteList_Draw(list_Sprites, back_buffer);
  
  skunkCONSOLEWRITE("Entering main loop.\n");
  
  while(true) {
    
    if(front_buffer == background_frame_0)
      {
	front_buffer = background_frame_1;
	back_buffer  = background_frame_0;
      }
    else
      {
	front_buffer = background_frame_0;
	back_buffer  = background_frame_1;
      }

    jag_wait_vbl();
    
    clear_video_buffer(back_buffer);

    /* Buffer is now clear. */
    
    framecounter = (framecounter + 1) % 60;
    framenumber++;

    if((framecounter % 60) == 0)
      {
	
      }

    BulletsList_Update(list_Bullets);
    
    /* Triggers once per frame while these are pressed */
    if(stick0_lastread & STICK_UP) {
      player_ship_coords.y = MAX(40, (player_ship_coords.y - 1));
    }
    if(stick0_lastread & STICK_DOWN) {
      player_ship_coords.y = MIN(160, (player_ship_coords.y + 1));
    }
    if(stick0_lastread & STICK_LEFT) {
      player_ship_coords.x = MAX(10, (player_ship_coords.x - 1));
    }
    if(stick0_lastread & STICK_RIGHT) {
      player_ship_coords.x = MIN(300, (player_ship_coords.x + 1));
    }
 
    stick0 = jag_read_stick0(STICK_READ_ALL);
    /* Debounced - only triggers once per press */
    switch(stick0 ^ stick0_lastread)
      {
      case STICK_UP:
	//if(~stick0_lastread & STICK_UP) printf("Up\n");
	break;
      case STICK_DOWN:
	//if(~stick0_lastread & STICK_DOWN) printf("Down\n");
	break;
      case STICK_LEFT:
	if(~stick0_lastread & STICK_LEFT)
	  {
	  }
	break;
      case STICK_RIGHT:
	if(~stick0_lastread & STICK_RIGHT)
	  {
	  }
	break;
      case STICK_A:
	//if(~stick0_lastread & STICK_A) printf("A\n");
	if(~stick0_lastread & STICK_A)
	  {
	    Coordinate bullet_coords = { .x = player_ship_coords.x + 6, .y = player_ship_coords.y };
	    AddHead(list_Bullets, BulletNode_Create(bullet_coords,
						    SPRITES_find("PulseBullet1"),
						    (Coordinate){ .x = 0, .y = -4 },
						    true)
		    );
	  }
	break;
      case STICK_B:
	//if(~stick0_lastread & STICK_B) printf("B\n");
	break;
      case STICK_C:
	//if(~stick0_lastread & STICK_C) printf("C\n");
	break;
      }
	  
    stick0_lastread = stick0;
    
    SpriteNode *player_ship = (SpriteNode *)FindName(list_Sprites, "PLAYER_SHIP");
    player_ship->location = (Coordinate){ .x = player_ship_coords.x, .y = player_ship_coords.y };

    draw_status_bar(back_buffer);

    //BLIT_8x8_text_string(text_buffer, 32, 16, "BLITTING");

    //BulletsList_Draw(list_Bullets, back_buffer);
    SpriteList_Draw(list_Sprites, back_buffer);

    jag_gpu_wait();

    jag_wait_blitter_ready();

    /*
    sprintf(skunkoutput, "R00 %08X R01 %08X R02 %08X R03 %08X\n", gpu_register_dump[0], gpu_register_dump[1], gpu_register_dump[2], gpu_register_dump[3]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R04 %08X R05 %08X R06 %08X R07 %08X\n", gpu_register_dump[4], gpu_register_dump[5], gpu_register_dump[6], gpu_register_dump[7]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R08 %08X R09 %08X R10 %08X R11 %08X\n", gpu_register_dump[8], gpu_register_dump[9], gpu_register_dump[10], gpu_register_dump[11]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R12 %08X R13 %08X R14 %08X R15 %08X\n", gpu_register_dump[12], gpu_register_dump[13], gpu_register_dump[14], gpu_register_dump[15]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R16 %08X R17 %08X R18 %08X R19 %08X\n", gpu_register_dump[16], gpu_register_dump[17], gpu_register_dump[18], gpu_register_dump[19]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R20 %08X R21 %08X R22 %08X R23 %08X\n", gpu_register_dump[20], gpu_register_dump[21], gpu_register_dump[22], gpu_register_dump[23]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R24 %08X R25 %08X R26 %08X R28 %08X\n", gpu_register_dump[24], gpu_register_dump[25], gpu_register_dump[26], gpu_register_dump[27]);
    skunkCONSOLEWRITE(skunkoutput);
    sprintf(skunkoutput, "R28 %08X R29 %08X R30 %08X R31 %08X\n", gpu_register_dump[28], gpu_register_dump[29], gpu_register_dump[30], gpu_register_dump[31]);
    skunkCONSOLEWRITE(skunkoutput);
    */
  }
}
