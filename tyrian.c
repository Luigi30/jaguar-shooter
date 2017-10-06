/* Note: All malloc() objects or anything on the heap is phrase-aligned.
   Objects on the stack are word-aligned. */

#include "tyrian.h"

char skunkoutput[256];

/*******/

typedef struct bullet_t {
  struct bullet_t *next;
  
  Coordinate location;
  SpriteGraphic image;
  Coordinate deltaPerFrame;
  bool isPlayer;
} Bullet;
Bullet *bullets_list;

Bullet *Bullet_Create(Coordinate _location, SpriteGraphic _image, Coordinate _deltaPerFrame, bool _isPlayer)
{
  Bullet *bullet = calloc(1, sizeof(Bullet));

  bullet->next = NULL;
  bullet->location = (Coordinate){ .x = _location.x, .y = _location.y };
  bullet->image = (SpriteGraphic){ .location = _image.location, .size = _image.size };
  bullet->deltaPerFrame = (Coordinate){ .x = _deltaPerFrame.x, .y = _deltaPerFrame.y };
  bullet->isPlayer = _isPlayer;

  return bullet;
}

void Bullets_Insert(Bullet **head, Bullet *new)
{
  new->next = *head;
  *head = new;
}

void Bullets_Update(Bullet **head)
{
  Bullet *current = NULL;
  Bullet *previous = NULL;

  if(*head != NULL){
    //skunkCONSOLEWRITE("*** start bullet list ***\n");

    for(current = *head; current != NULL; previous = current, current = current->next)
      {	
	//	sprintf(skunkoutput, "bullet: addr %08p, location (%03d,%03d)\n", current, current->location.x, current->location.y);
	//skunkCONSOLEWRITE(skunkoutput);

	current->location.y += current->deltaPerFrame.y;
	if(current->location.y < 10)
	  {
	    if(previous != NULL)
	      {
		//If there's a previous node, set its next ptr to current->next
		previous->next = current->next;
	      }
	    else 
	      {
		//If there is no previous node, then the list's head becomes current->next.
		*head = current->next;
	      }

	    //Free the current node.
	    Bullet *temp = current;
	    free(temp);

	    //Update the current ptr so we advance to the next node if one exists.
	    current = previous;
	    
	    if(current == NULL){
	      break; //If there's no current ptr, end list processing.
	    }
	  }
      }

    //skunkCONSOLEWRITE("*** end of bullet list ***\n");
  }
}

void Bullets_Draw(Bullet **head)
{
  Bullet *current = *head;

  while(current != NULL) {
    GPU_do_blit_sprite(back_buffer, current->location, shipsheet, SPRITES_find("PulseBullet1"));

    current = current->next;
  }
  
}

/*******/

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

      mobj_background.graphic->p0.data = (uint32_t)front_buffer >> 3;
      mobj_font.graphic->p0.data = (uint32_t)text_buffer >> 3;

      MMIO16(INT2) = 0;
      return C_VIDCLR;
    }
  return 0;
}



void blit_sprite(uint8_t *destination, Coordinate destination_coordinate, uint8_t *source, const SpriteGraphic *sprite)
{  
  jag_wait_blitter_ready();
  
  //the sprite sheet is 232x672 8bpp
  //the destination bitmap is 320x200 8bpp
  
  MMIO32(A1_BASE)   = (long)destination;
  MMIO32(A1_PIXEL)  = BLIT_XY(destination_coordinate.x, destination_coordinate.y);
  MMIO32(A1_FPIXEL) = 0;
  MMIO32(A1_FLAGS)  = PITCH1 | PIXEL8 | WID320 | XADDPIX | YADD0;
  MMIO32(A1_STEP)   = BLIT_XY(320-sprite->size.x, 0);
  MMIO32(A1_CLIP)   = BLIT_XY(320, 200);
  
  MMIO32(A2_BASE)   = (long)source;
  MMIO32(A2_PIXEL)  = BLIT_XY(sprite->location.x, sprite->location.y);
  MMIO32(A2_STEP)   = BLIT_XY(256-sprite->size.x, 0);
  MMIO32(A2_FLAGS)  = PITCH1 | PIXEL8 | WID256 | XADDPIX | YADD0;
  
  MMIO32(B_COUNT)   = BLIT_XY(sprite->size.x, sprite->size.y);

  //SRCEN and DSTEN must be enabled for blits below 8bpp
  MMIO32(B_CMD)     = SRCEN | DSTEN | UPDA1 | UPDA2 | DCOMPEN | LFU_REPLACE;
}

void clear_video_buffer(uint8_t *buffer){
  BLIT_rectangle_solid(buffer, 0, 0, 320, 200, 0);
}

int main() {
  //set correct endianness
  MMIO32(G_END) = 0x00070007;
  
  srand(8675309);
  jag_console_hide();
  
  BLIT_init_blitter();
  
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

  BLIT_8x8_text_string(text_buffer, 32, 180, "SPR_BigRedBadGuy");
  BLIT_8x8_text_string(text_buffer, 32, 188, "SPR_USPTalon");

  int current_palette = 0;
  char palette_string[32];

  Coordinate player_ship_coords = (Coordinate){ .x = 100, .y = 140 };
  int player_side_movement_frames = 0;

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

    Bullets_Update(&bullets_list);
    
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
	    Bullets_Insert(&bullets_list,
			  Bullet_Create(bullet_coords, SPR_PulseBullet_1, (Coordinate){ .x = 0, .y = -4 }, true)
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

    {
      Coordinate destination = { .x = 80, .y = 0 };
      GPU_do_blit_sprite(back_buffer, destination, shipsheet, SPRITES_find("BigRedBadGuy"));
    }

    GPU_do_blit_sprite(back_buffer, player_ship_coords, shipsheet, SPRITES_find("USPTalon"));

    Bullets_Draw(&bullets_list);
    
    /*
    if(bullets_list != NULL)
      {
	Bullet *bullet = bullets_list;
	bullet->location.y += bullet->deltaPerFrame.y;
	if(bullet->location.y > 0) {
	  GPU_do_blit_sprite(back_buffer, bullet->location, shipsheet, SPRITES_find("PulseBullet1"));
	}
	else
	  bullets_list = NULL;
      }
    */
    
    //MOBJ_Print_Position(mobj_lottoballs[0]);
  }
}

