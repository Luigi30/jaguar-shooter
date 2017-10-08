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

extern char skunkoutput[256];

typedef struct spriteentry_t {
  struct spriteentry_t *next;
  
  Coordinate location;
  SpriteGraphic image;
  Coordinate deltaPerFrame;
  bool isPlayer;
} SpriteEntry;

extern SpriteEntry *SpriteDisplay_List;

/* Display list functions */
SpriteEntry *SpriteEntry_Create(Coordinate _location, SpriteGraphic *_image, Coordinate _deltaPerFrame, bool _isPlayer);
void SpriteEntry_Insert(SpriteEntry **head, SpriteEntry *new);
void SpriteEntry_Update(SpriteEntry **head);
void SpriteEntry_Draw(SpriteEntry **head, uint8_t *buffer);

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
extern uint8_t GPU_process_display_list[];

extern SpriteEntry 	*GPU_sprite_display_list_head;
extern SpriteEntry 	*GPU_sprite_display_list_current;

extern uint8_t 		*GPU_blit_destination;
extern Coordinate 	GPU_blit_destination_coordinate;
extern uint8_t 		*GPU_blit_source;
extern const SpriteGraphic *GPU_blit_sprite;

#endif
