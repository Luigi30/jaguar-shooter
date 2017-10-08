#include "sprite.h"

SpriteEntry *SpriteDisplay_List = NULL;

/* Display list functions */
SpriteEntry *SpriteEntry_Create(Coordinate _location, SpriteGraphic *_image, Coordinate _deltaPerFrame, bool _isPlayer)
{
  SpriteEntry *sprite_entry = calloc(1, sizeof(SpriteEntry));

  sprite_entry->next = NULL;
  sprite_entry->location = (Coordinate){ .x = _location.x, .y = _location.y };
  sprite_entry->image = (SpriteGraphic){ .location = _image->location, .size = _image->size };
  sprite_entry->deltaPerFrame = (Coordinate){ .x = _deltaPerFrame.x, .y = _deltaPerFrame.y };
  sprite_entry->isPlayer = _isPlayer;

  strcpy(sprite_entry->image.name, _image->name);

  return sprite_entry;
}

void SpriteEntry_Insert(SpriteEntry **head, SpriteEntry *new)
{
  new->next = *head;
  *head = new;
}

void SpriteEntry_Draw(SpriteEntry **head, uint8_t *buffer)
{
  //GPU_sprite_display_list_head = *head;
  //GPU_START(GPU_process_display_list);

  SpriteEntry *current = *head;

  while(current != NULL) {
    GPU_do_blit_sprite(buffer, current->location, shipsheet, SPRITES_find(current->image.name));

    current = current->next;
  }

}

/* GPU functions */
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
  &SPR_NME_GrayJet,
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
