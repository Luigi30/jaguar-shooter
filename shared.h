#ifndef SHARED_H
#define SHARED_H

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MMIO16(x)   (*(vuint16_t *)(x))
#define MMIO32(x)   (*(vuint32_t *)(x))
#define MMIO64(x)   (*(vuint64_t *)(x))

typedef struct coordinate_t {
  uint16_t x, y;
} Coordinate;

typedef Coordinate Size2D;

typedef struct spritegraphic_t {
  char name[16];
  Coordinate location; //Upper left coordinate of the sprite on the sheet
  Size2D size;         //Size of the sprite.
} SpriteGraphic;

#endif
