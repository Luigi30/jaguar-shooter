#ifndef SHIPSHEET_H
#define SHIPSHEET_H

#include "shared.h"
#include "string.h"
#include "jaglib.h"

static const SpriteGraphic SPR_USPTalon = { .name = "USPTalon",
					    .location = { .x = 48, .y = 170 },
					    .size = { .x = 24, .y = 25 } };

static const SpriteGraphic SPR_BigRedBadGuy = { .name = "BigRedBadGuy",
						.location = { .x = 0, .y = 337 },
						.size = { .x = 144, .y = 111 } };
						
static const SpriteGraphic SPR_NME_GrayJet = { .name = "NME_GrayJet",
						.location = { .x = 24, .y = 280 },
						.size = { .x = 24, .y = 28 } };

/* bullets */
static const SpriteGraphic SPR_PulseBullet_1 = { .name = "PulseBullet1",
						 .location = { .x = 0, .y = 490 },
						 .size = { .x = 12, .y = 14 } };
static const SpriteGraphic SPR_PulseBullet_2 = { .name = "PulseBullet2",
						 .location = { .x = 12, .y = 490 },
						 .size = { .x = 12, .y = 14 } };
static const SpriteGraphic SPR_PulseBullet_3 = { .name = "PulseBullet3",
						 .location = { .x = 24, .y = 490 },
						 .size = { .x = 12, .y = 14 } };
static const SpriteGraphic SPR_PulseBullet_4 = { .name = "PulseBullet4",
						 .location = { .x = 36, .y = 490 },
						 .size = { .x = 12, .y = 14 } };
static const SpriteGraphic SPR_PulseBullet_5 = { .name = "PulseBullet5",
						 .location = { .x = 48, .y = 490 },
						 .size = { .x = 12, .y = 14 } };

#endif
