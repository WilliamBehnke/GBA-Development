#include "world_map.h"

#include "bn_regular_bg_tiles_items_tiles.h"
#include "bn_bg_palette_items_palette.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_math.h"

WorldMap::WorldMap()
{
    _build_room(_current_room);
}

void WorldMap::_fill_layer(const uint8_t* source, BgLayer& layer)
{
    const int half_width  = ROOM_WIDTH  / 2;
    const int half_height = ROOM_HEIGHT / 2;
    constexpr int empty_tile_index = 40;
    int j = 0;

    auto fill_quad = [&](int y_start, int y_end, int x_start, int x_end)
    {
        for(int y = y_start; y < y_end; ++y)
        {
            for(int x = x_start; x < x_end; ++x, ++j)
            {
                const int index = y * ROOM_WIDTH + x;
                bn::regular_bg_map_cell_info cell_info;
                const int src_value = source[index];

                if(src_value == 0)
                {
                    cell_info.set_tile_index(empty_tile_index);
                }
                else
                {
                    cell_info.set_tile_index(src_value - 1);
                }

                layer.cells[j] = cell_info.cell();
            }
        }
    };

    // Q1: top-left
    fill_quad(0,           half_height,     0,           half_width);
    // Q2: top-right
    fill_quad(0,           half_height,     half_width,  ROOM_WIDTH);
    // Q3: bottom-left
    fill_quad(half_height, ROOM_HEIGHT,     0,           half_width);
    // Q4: bottom-right
    fill_quad(half_height, ROOM_HEIGHT,     half_width,  ROOM_WIDTH);
}

void WorldMap::_build_room(RoomId room)
{
    _current_room = room;
    const RoomData& data = g_rooms[static_cast<int>(room)];

    // -----------------------------
    // Layer 1: ground / base
    // -----------------------------
    _fill_layer(data.layer1, _layer1_map);

    bn::regular_bg_item bg_item1(
        bn::regular_bg_tiles_items::tiles,
        bn::bg_palette_items::palette,
        _layer1_map.map_item);

    _layer1_bg = bg_item1.create_bg(0, 0);
    _layer1_bg->set_priority(2);   // behind layer 2 & sprites

    // -----------------------------
    // Layer 2: decorations / above
    // -----------------------------
    _fill_layer(data.layer2, _layer2_map);

    bn::regular_bg_item bg_item2(
        bn::regular_bg_tiles_items::tiles,
        bn::bg_palette_items::palette,
        _layer2_map.map_item);

    _layer2_bg = bg_item2.create_bg(0, 0);
    _layer2_bg->set_priority(0);   // in front of layer 1, behind sprites

    // -----------------------------
    // Collision layer
    // -----------------------------
    for(int i = 0; i < ROOM_CELLS; ++i)
    {
        // 0 = empty, non-zero = solid
        _collision_cells[i] = data.collision[i];
    }

    // -----------------------------
    // Doors for this room (from RoomData::doors)
    // -----------------------------
    _doors.clear();

    const int map_px_w = ROOM_WIDTH * TILE_SIZE;
    const int map_px_h = ROOM_HEIGHT * TILE_SIZE;

    const int left_px = -map_px_w / 2;
    const int top_px  = -map_px_h / 2;

    for(int i = 0; i < data.door_count; ++i)
    {
        const DoorData& dd = data.doors[i];

        // Tile rectangle -> pixel center
        // door spans [tile_x .. tile_x + width_tiles - 1]
        const int door_px_center_x =
            left_px + dd.tile_x * TILE_SIZE + (dd.width_tiles * TILE_SIZE) / 2;
        const int door_px_center_y =
            top_px + dd.tile_y * TILE_SIZE + TILE_SIZE / 2;

        bn::fixed_point door_center{
            bn::fixed(door_px_center_x),
            bn::fixed(door_px_center_y)
        };

        // Half-width/height in pixels
        const bn::fixed half_width  = bn::fixed(dd.width_tiles * TILE_SIZE) / 2;
        const bn::fixed half_height = 6;

        // Spawn position in target room (pixel coordinates, room-centered)
        bn::fixed_point spawn_pos{
            bn::fixed(dd.target_x_px),
            bn::fixed(dd.target_y_px)
        };

        if(!_doors.full())
        {
            _doors.push_back(Door(
                door_center,
                half_width,
                half_height,
                dd.target_room,
                spawn_pos
            ));
        }
        // else: silently ignore extra doors if MAX_DOORS_PER_ROOM is exceeded
    }

    // Reattach camera to BGs + doors if we already have one
    if(_camera)
    {
        set_camera(_camera.value());
    }
}

// Attach camera to both BG layers and all doors:
void WorldMap::set_camera(const bn::camera_ptr& camera)
{
    _camera = camera;

    if(_layer1_bg)
    {
        _layer1_bg->set_camera(_camera.value());
    }

    if(_layer2_bg)
    {
        _layer2_bg->set_camera(_camera.value());
    }

    for(Door& door : _doors)
    {
        door.set_camera(_camera);
    }
}

// Map is centered at (0,0).
// World X/Y in pixels; convert to tile coordinates and look up collision.
bool WorldMap::is_solid(const bn::fixed_point& world_pos) const
{
    int map_px_w = ROOM_WIDTH * TILE_SIZE;
    int map_px_h = ROOM_HEIGHT * TILE_SIZE;

    int left_px = -map_px_w / 2;
    int top_px  = -map_px_h / 2;

    int x_px = world_pos.x().integer();
    int y_px = world_pos.y().integer();

    int tx = (x_px - left_px) / TILE_SIZE;
    int ty = (y_px - top_px) / TILE_SIZE;

    if(tx < 0 || tx >= ROOM_WIDTH || ty < 0 || ty >= ROOM_HEIGHT)
    {
        // Outside the map = solid wall
        return true;
    }

    int index = ty * ROOM_WIDTH + tx;
    return _collision_cells[index] != 0;
}

int WorldMap::pixel_width() const
{
    return ROOM_WIDTH * TILE_SIZE;
}

int WorldMap::pixel_height() const
{
    return ROOM_HEIGHT * TILE_SIZE;
}

void WorldMap::change_room(RoomId room)
{
    if(room == _current_room)
        return;

    _build_room(room);
}

bn::optional<DoorTarget> WorldMap::check_door_collision(const bn::fixed_point& player_pos) const
{
    for(const Door& door : _doors)
    {
        if(door.contains(player_pos))
        {
            return door.target();
        }
    }
    return bn::nullopt;
}

void WorldMap::update()
{
    for(Door& door : _doors)
    {
        door.update();
    }
}
