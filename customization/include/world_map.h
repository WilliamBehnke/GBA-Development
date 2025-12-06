#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_array.h"
#include "bn_regular_bg_map_item.h"
#include "world_map_data.h"

class WorldMap
{
public:
    // Create world map from compiled tile/collision data
    WorldMap();

    // Attach a camera so the map scrolls with it
    void set_camera(const bn::camera_ptr& camera);

    // Collision query at a world position (in pixels, centered map)
    bool is_solid(const bn::fixed_point& world_pos) const;

    // Map size in pixels (used for camera clamping)
    int pixel_width() const;
    int pixel_height() const;

    // Current room
    RoomId current_room() const { return _current_room; }

    // Change to another room
    void change_room(RoomId room);

    // Per-frame update
    void update();

private:
    struct BgLayer
    {
        alignas(int) bn::regular_bg_map_cell cells[ROOM_CELLS];
        bn::regular_bg_map_item map_item;

        BgLayer() :
            map_item(cells[0], bn::size(ROOM_WIDTH, ROOM_HEIGHT))
        {
        }
    };

    // Visible layers:
    BgLayer _layer1_map;
    BgLayer _layer2_map;

    bn::optional<bn::regular_bg_ptr> _layer1_bg;
    bn::optional<bn::regular_bg_ptr> _layer2_bg;

    // Collision layer (0 = empty, non-zero = solid)
    bn::array<uint8_t, ROOM_CELLS> _collision_cells;

    // Camera (we don’t move it here; Player does. We just attach it.)
    bn::optional<bn::camera_ptr> _camera;

    // Which room is currently loaded
    RoomId _current_room = RoomId::MainRoom;

    // Internal helpers
    void _build_room(RoomId room);
    void _fill_layer(const uint16_t* source, BgLayer& layer);
};

#endif // WORLD_MAP_H
