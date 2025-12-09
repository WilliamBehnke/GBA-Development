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

// -----------------------------------------------------------------------------
// Room IDs
// -----------------------------------------------------------------------------
enum class RoomId : int
{
    MainRoom = 0,
    Hallway  = 1,
    Room2    = 2,
    Room3    = 3,
    Room4    = 4,
    Room5    = 5,
    Room6    = 6,
    Room7    = 7,
    Room8    = 8,
    Room9    = 9,
    Room10   = 10,
    Room11   = 11,
    Room12   = 12,
    Room13   = 13,
    Room14   = 14,
};

constexpr int ROOM_COUNT = 16;

// -----------------------------------------------------------------------------
// Door data (pure data, no sprites)
// -----------------------------------------------------------------------------
struct DoorData
{
    // Door rectangle in TILE coordinates within this room:
    // - tile_x / tile_y are the LEFTMOST tile and row of the door
    // - width_tiles is how many tiles wide (you said 4)
    uint8_t tile_x;       // 0..63
    uint8_t tile_y;       // 0..63
    uint8_t width_tiles;  // typically 4

    // Which room this door leads to
    RoomId target_room;

    // Spawn position in the target room (in PIXELS, room-centered)
    // (world coordinates, just like your player/world map use)
    int16_t target_x_px;
    int16_t target_y_px;
};

// -----------------------------------------------------------------------------
// Per-room tile/collision/door data
// -----------------------------------------------------------------------------
struct RoomData
{
    const uint8_t* layer1;      // bottom
    const uint8_t* layer2;      // top
    const bool* collision;      // 0 = empty, non-zero = solid

    // Doors defined for this room:
    const DoorData* doors;
    int             door_count;
};

// Expose the list of rooms:
extern const RoomData g_rooms[ROOM_COUNT];

// -----------------------------------------------------------------------------
// Example room arrays (you’ll define them in world_map_data.cpp)
// -----------------------------------------------------------------------------

// Room 0 (MainRoom)
extern const uint8_t room0_layer1[ROOM_CELLS];
extern const uint8_t room0_layer2[ROOM_CELLS];
extern const bool    room0_collision[ROOM_CELLS];

extern const DoorData room0_doors[];
extern const int      room0_door_count;

// Room 1 (Hallway)
extern const uint8_t room1_layer1[ROOM_CELLS];
extern const uint8_t room1_layer2[ROOM_CELLS];
extern const bool    room1_collision[ROOM_CELLS];

extern const DoorData room1_doors[];
extern const int      room1_door_count;

// Room 2
extern const uint8_t room2_layer1[ROOM_CELLS];
extern const uint8_t room2_layer2[ROOM_CELLS];
extern const bool    room2_collision[ROOM_CELLS];

extern const DoorData room2_doors[];
extern const int      room2_door_count;

// Room 3
extern const uint8_t room3_layer1[ROOM_CELLS];
extern const uint8_t room3_layer2[ROOM_CELLS];
extern const bool    room3_collision[ROOM_CELLS];

extern const DoorData room3_doors[];
extern const int      room3_door_count;

// Room 4
extern const uint8_t room4_layer1[ROOM_CELLS];
extern const uint8_t room4_layer2[ROOM_CELLS];
extern const bool    room4_collision[ROOM_CELLS];

extern const DoorData room4_doors[];
extern const int      room4_door_count;

// Room 5
extern const uint8_t room5_layer1[ROOM_CELLS];
extern const uint8_t room5_layer2[ROOM_CELLS];
extern const bool    room5_collision[ROOM_CELLS];

extern const DoorData room5_doors[];
extern const int      room5_door_count;

// Room 6
extern const uint8_t room6_layer1[ROOM_CELLS];
extern const uint8_t room6_layer2[ROOM_CELLS];
extern const bool    room6_collision[ROOM_CELLS];

extern const DoorData room6_doors[];
extern const int      room6_door_count;

// Room 7
extern const uint8_t room7_layer1[ROOM_CELLS];
extern const uint8_t room7_layer2[ROOM_CELLS];
extern const bool    room7_collision[ROOM_CELLS];

extern const DoorData room7_doors[];
extern const int      room7_door_count;

// Room 8
extern const uint8_t room8_layer1[ROOM_CELLS];
extern const uint8_t room8_layer2[ROOM_CELLS];
extern const bool    room8_collision[ROOM_CELLS];

extern const DoorData room8_doors[];
extern const int      room8_door_count;

// Room 9
extern const uint8_t room9_layer1[ROOM_CELLS];
extern const uint8_t room9_layer2[ROOM_CELLS];
extern const bool    room9_collision[ROOM_CELLS];

extern const DoorData room9_doors[];
extern const int      room9_door_count;

// Room 10
extern const uint8_t room10_layer1[ROOM_CELLS];
extern const uint8_t room10_layer2[ROOM_CELLS];
extern const bool    room10_collision[ROOM_CELLS];

extern const DoorData room10_doors[];
extern const int      room10_door_count;

// Room 11
extern const uint8_t room11_layer1[ROOM_CELLS];
extern const uint8_t room11_layer2[ROOM_CELLS];
extern const bool    room11_collision[ROOM_CELLS];

extern const DoorData room11_doors[];
extern const int      room11_door_count;

// Room 12
extern const uint8_t room12_layer1[ROOM_CELLS];
extern const uint8_t room12_layer2[ROOM_CELLS];
extern const bool    room12_collision[ROOM_CELLS];

extern const DoorData room12_doors[];
extern const int      room12_door_count;

// Room 13
extern const uint8_t room13_layer1[ROOM_CELLS];
extern const uint8_t room13_layer2[ROOM_CELLS];
extern const bool    room13_collision[ROOM_CELLS];

extern const DoorData room13_doors[];
extern const int      room13_door_count;

// Room 14
extern const uint8_t room14_layer1[ROOM_CELLS];
extern const uint8_t room14_layer2[ROOM_CELLS];
extern const bool    room14_collision[ROOM_CELLS];

extern const DoorData room14_doors[];
extern const int      room14_door_count;

// Room 15
extern const uint8_t room15_layer1[ROOM_CELLS];
extern const uint8_t room15_layer2[ROOM_CELLS];
extern const bool    room15_collision[ROOM_CELLS];

extern const DoorData room15_doors[];
extern const int      room15_door_count;

#endif // WORLD_MAP_DATA_H
