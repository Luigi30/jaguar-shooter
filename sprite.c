#include "sprite.h"

/* GPU */
void GPU_LOAD_SPRITE_PROGRAM() {
  skunkCONSOLEWRITE("GPU_LOAD_SPRITE_PROGRAM(): beginning upload\n");
  jag_dsp_load(G_RAM, gpu_sprite_program, gpu_sprite_program_end-gpu_sprite_program);
  skunkCONSOLEWRITE("GPU_LOAD_SPRITE_PROGRAM(): upload complete\n");
}

void GPU_START(uint8_t *function) {
  MMIO32(G_PC) = (uint32_t)(0xF03000 + function - gpu_sprite_program_start);
  
  /*
  char gpustr[256];
  sprintf(gpustr, "GPU_START(): function is at $%08X. gpu_sprite_program_start is at $%08X\n", (long)function, (long)gpu_sprite_program_start);
  skunkCONSOLEWRITE(gpustr);
  
  sprintf(gpustr, "GPU_START(): GPU running from $%08X\n", (uint32_t)(0xF03000 + function - gpu_sprite_program_start));
  skunkCONSOLEWRITE(gpustr);
  */
  
  MMIO32(G_CTRL) = MMIO32(G_CTRL) | 0x01;
}

#define GPU_WAIT() ( jag_gpu_wait() )

const SpriteGraphic * SPRITES_LIST[] = {
  &SPR_USPTalon,
  &SPR_BigRedBadGuy,
  &SPR_PulseBullet_1,
  &SPR_PulseBullet_2,
  &SPR_PulseBullet_3,
  &SPR_PulseBullet_4,
  &SPR_PulseBullet_5,
  NULL
};

const SpriteGraphic * SPRITES_find(char *name)
{ 
  int index = 0;

  while(SPRITES_LIST[index] != NULL){
    if(strcmp(SPRITES_LIST[index]->name, name) == 0) {
      return SPRITES_LIST[index];
    } else {
      index++;
    }
  }

  char skunk[256];
  sprintf(skunk, "SPRITES_find(): could not find sprite '%s'\n", name);
  skunkCONSOLEWRITE(skunk);
  return NULL;
}

void GPU_do_blit_sprite(uint8_t *destination, Coordinate destination_coordinate, uint8_t *source, const SpriteGraphic *sprite)
{
  jag_gpu_wait();
  
  GPU_blit_destination = destination;
  GPU_blit_destination_coordinate = (Coordinate){ .x = destination_coordinate.x, .y = destination_coordinate.y };
  GPU_blit_source = source;
  GPU_blit_sprite = sprite;

  GPU_START(gpu_sprite_test);
}