#include "world_map.h"

#include "bn_regular_bg_tiles_items_tiles.h"
#include "bn_bg_palette_items_palette.h"
#include "bn_regular_bg_item.h"

#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_map_item.h"
#include "bn_regular_bg_ptr.h"
#include "bn_math.h"

WorldMap::WorldMap()
{
    _build_layers_from_data();
}

void WorldMap::_fill_layer(const uint16_t* source, BgLayer& layer)
{
    const int half_width  = WORLD_MAP_WIDTH  / 2;
    const int half_height = WORLD_MAP_HEIGHT / 2;
    constexpr int empty_tile_index = 10;
    int j = 0;

    auto fill_quad = [&](int y_start, int y_end, int x_start, int x_end)
    {
        for(int y = y_start; y < y_end; ++y)
        {
            for(int x = x_start; x < x_end; ++x, ++j)
            {
                const int index = y * WORLD_MAP_WIDTH + x;
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
    fill_quad(0,           half_height,     half_width,  WORLD_MAP_WIDTH);
    // Q3: bottom-left
    fill_quad(half_height, WORLD_MAP_HEIGHT, 0,           half_width);
    // Q4: bottom-right
    fill_quad(half_height, WORLD_MAP_HEIGHT, half_width,  WORLD_MAP_WIDTH);
};

void WorldMap::_build_layers_from_data()
{
    // -----------------------------
    // Layer 1: ground / base
    // -----------------------------
    _fill_layer(world_layer1, _layer1_map);

    bn::regular_bg_item bg_item1(
        bn::regular_bg_tiles_items::tiles,
        bn::bg_palette_items::palette,
        _layer1_map.map_item);

    _layer1_bg = bg_item1.create_bg(0, 0);
    _layer1_bg->set_priority(2);   // behind layer 2 & sprites

    // -----------------------------
    // Layer 2: decorations / above
    // -----------------------------
    _fill_layer(world_layer2, _layer2_map);

    bn::regular_bg_item bg_item2(
        bn::regular_bg_tiles_items::tiles,
        bn::bg_palette_items::palette,
        _layer2_map.map_item);

    _layer2_bg = bg_item2.create_bg(0, 0);
    _layer2_bg->set_priority(1);   // in front of layer 1, behind sprites

    // -----------------------------
    // Collision layer
    // -----------------------------
    for(int i = 0; i < WORLD_MAP_CELLS; ++i)
    {
        _collision_cells[i] = world_collision[i];    // 0 = empty, non-zero = solid
    }
}

// Attach camera to both BG layers:
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
}

// Map is centered at (0,0).
// World X/Y in pixels; convert to tile coordinates and look up collision.
bool WorldMap::is_solid(const bn::fixed_point& world_pos) const
{
    int map_px_w = WORLD_MAP_WIDTH * TILE_SIZE;
    int map_px_h = WORLD_MAP_HEIGHT * TILE_SIZE;

    int left_px = -map_px_w / 2;
    int top_px  = -map_px_h / 2;

    int x_px = world_pos.x().integer();
    int y_px = world_pos.y().integer();

    int tx = (x_px - left_px) / TILE_SIZE;
    int ty = (y_px - top_px) / TILE_SIZE;

    if(tx < 0 || tx >= WORLD_MAP_WIDTH || ty < 0 || ty >= WORLD_MAP_HEIGHT)
    {
        // Outside the map = solid wall
        return true;
    }

    int index = ty * WORLD_MAP_WIDTH + tx;
    return _collision_cells[index] != 0;
}

int WorldMap::pixel_width() const
{
    return WORLD_MAP_WIDTH * TILE_SIZE;
}

int WorldMap::pixel_height() const
{
    return WORLD_MAP_HEIGHT * TILE_SIZE;
}
