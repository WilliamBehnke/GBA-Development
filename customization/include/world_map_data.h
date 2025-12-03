#ifndef WORLD_MAP_DATA_H
#define WORLD_MAP_DATA_H

#include <stdint.h>

constexpr int TILE_SIZE        = 8;
constexpr int WORLD_MAP_WIDTH  = 64;
constexpr int WORLD_MAP_HEIGHT = 64;
constexpr int WORLD_MAP_CELLS  = WORLD_MAP_WIDTH * WORLD_MAP_HEIGHT;

extern const uint16_t world_layer1[WORLD_MAP_CELLS];
extern const uint16_t world_layer2[WORLD_MAP_CELLS];
extern const uint32_t world_collision[WORLD_MAP_CELLS];

#endif // WORLD_MAP_DATA_H
