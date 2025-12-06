#ifndef WORLD_MAP_DATA_H
#define WORLD_MAP_DATA_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// Shared constants: each room is 64x64 tiles of size 8x8
// -----------------------------------------------------------------------------
constexpr int TILE_SIZE   = 8;
constexpr int ROOM_WIDTH  = 64;
constexpr int ROOM_HEIGHT = 64;
constexpr int ROOM_CELLS  = ROOM_WIDTH * ROOM_HEIGHT;

// ID for rooms. You can add more as needed.
enum class RoomId : int
{
    MainRoom = 0,
    Hallway  = 1,
    // Add more...
};

constexpr int ROOM_COUNT = 2;   // adjust when you add more rooms

// -----------------------------------------------------------------------------
// Per-room tile/collision data
// -----------------------------------------------------------------------------
struct RoomData
{
    const uint16_t* layer1;       // bottom
    const uint16_t* layer2;       // top
    const uint8_t* collision;     // 0 = empty, non-zero = solid
};

// Expose the list of rooms:
extern const RoomData g_rooms[ROOM_COUNT];

// Example room arrays (you’ll define them in the .cpp)
extern const uint16_t room0_layer1[ROOM_CELLS];
extern const uint16_t room0_layer2[ROOM_CELLS];
extern const uint8_t room0_collision[ROOM_CELLS];

extern const uint16_t room1_layer1[ROOM_CELLS];
extern const uint16_t room1_layer2[ROOM_CELLS];
extern const uint8_t room1_collision[ROOM_CELLS];

#endif // WORLD_MAP_DATA_H
