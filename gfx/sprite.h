#ifndef SPRITE_H
#define SPRITE_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "jaglib.h"
#include "jagcore.h"

#include "shared.h"
#include "images.h"

#include "spritesheets/shipsheet.h"
#include "utils/list.h"

extern char skunkoutput[256];

typedef struct sprite_node_t {
  struct Node node;
	
  uint16_t padding; //make the rest of the node long-aligned.
  Coordinate location;
  SpriteGraphic image;
  Coordinate deltaPerFrame;
  bool isPlayer;
  uint8_t end_padding;
} SpriteNode;

extern struct List *list_Sprites;

/* Display list functions */
SpriteNode *SpriteNode_Create(Coordinate _location, SpriteGraphic *_image, Coordinate _deltaPerFrame, bool _isPlayer);
void SpriteList_Draw(struct List *spriteList, uint8_t *buffer);

/* Sprite graphics table */
const SpriteGraphic * SPRITES_find(char *name);
extern const SpriteGraphic * SPRITES_LIST[];

/* GPU functions */
void GPU_LOAD_SPRITE_PROGRAM();
void GPU_START(uint8_t *function);

void GPU_do_blit_sprite(uint8_t *destination, Coordinate destination_coordinate, uint8_t *source, const SpriteGraphic *sprite);

extern uint8_t gpu_sprite_program[];
extern uint8_t gpu_sprite_program_start[];
extern uint8_t gpu_sprite_program_end[];

extern uint8_t gpu_sprite_test[];
extern uint8_t gpu_process_sprite_list[];

extern uint8_t gpu_reset_bg_tile_objects[];

extern uint8_t 		*GPU_blit_destination;
extern Coordinate 	GPU_blit_destination_coordinate;
extern uint8_t 		*GPU_blit_source;
extern const SpriteGraphic *GPU_blit_sprite;

extern struct List *gpu_sprite_display_list;

#endif
