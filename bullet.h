#ifndef BULLET_H
#define BULLET_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "shared.h"
#include "jaglib.h"
#include "gfx/sprite.h"
#include "images.h"

typedef struct bullet_t {
  struct bullet_t *next;
  
  Coordinate location;
  SpriteGraphic image;
  Coordinate deltaPerFrame;
  bool isPlayer;
} Bullet;

extern Bullet *bullets_list;

Bullet *Bullet_Create(Coordinate _location, SpriteGraphic _image, Coordinate _deltaPerFrame, bool _isPlayer);
void Bullets_Insert(Bullet **head, Bullet *new);
void Bullets_Update(Bullet **head);
void Bullets_Draw(Bullet **head, uint8_t *buffer);

#endif
