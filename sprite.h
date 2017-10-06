#ifndef SPRITE_H
#define SPRITE_H

#include <stdio.h>
#include <string.h>
#include "jaglib.h"
#include "jagcore.h"

#include "shared.h"
#include "shipsheet.h"

extern char skunkoutput[256];

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

extern uint8_t 		*GPU_blit_destination;
extern Coordinate 	GPU_blit_destination_coordinate;
extern uint8_t 		*GPU_blit_source;
extern const SpriteGraphic *GPU_blit_sprite;

#endif
