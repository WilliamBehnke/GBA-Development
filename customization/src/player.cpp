#include "player.h"

#include "bn_keypad.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_math.h"

#include "world_map.h"

namespace
{
    // Same component lists as customization screen:
    constexpr int k_body_type_count = 1;
    const bn::sprite_item* k_body_type_options[k_body_type_count] =
    {
        &bn::sprite_items::skin_0,
    };

    constexpr int k_hair_count = 2;
    const bn::sprite_item* k_hair_options[k_hair_count] =
    {
        &bn::sprite_items::hair_0,
        &bn::sprite_items::hair_1,
    };

    constexpr int k_eyes_count = 1;
    const bn::sprite_item* k_eyes_options[k_eyes_count] =
    {
        &bn::sprite_items::eyes_0,
    };

    constexpr int k_top_count = 1;
    const bn::sprite_item* k_top_options[k_top_count] =
    {
        &bn::sprite_items::top_0,
    };

    constexpr int k_bottom_count = 1;
    const bn::sprite_item* k_bottom_options[k_bottom_count] =
    {
        &bn::sprite_items::bottom_0,
    };

    void apply_ramp_to_palette(const ColorRamp& ramp, bn::sprite_palette_ptr& pal)
    {
        pal.set_color(1, ramp.c0);
        pal.set_color(2, ramp.c1);
        pal.set_color(3, ramp.c2);
        pal.set_color(4, ramp.c3);
    }
}

// ---------------------------------------------------------------------------

Player::Player(const CharacterAppearance& appearance, const bn::fixed_point& start_pos) :
    _appearance(appearance),
    _pos(start_pos),
    _direction(_appearance.direction)
{
    _rebuild_sprites();
    _apply_colors();
    _sync_sprites();
}

// ---------------------------------------------------------------------------

void Player::attach_camera(const bn::camera_ptr& camera)
{
    _camera = camera;

    if(_body_sprite)
    {
        auto& cam = _camera.value();
        _body_sprite->set_camera(cam);
        _eyes_sprite->set_camera(cam);
        _top_sprite->set_camera(cam);
        _bottom_sprite->set_camera(cam);
        _hair_sprite->set_camera(cam);
    }
}

// ---------------------------------------------------------------------------

void Player::_handle_input()
{
    _move_dx = 0;
    _move_dy = 0;

    if(bn::keypad::left_held())
    {
        _move_dx -= k_speed;
        _direction = FacingDirection::Left;
    }
    else if(bn::keypad::right_held())
    {
        _move_dx += k_speed;
        _direction = FacingDirection::Right;
    }

    if(bn::keypad::up_held())
    {
        _move_dy -= k_speed;
        _direction = FacingDirection::Up;
    }
    else if(bn::keypad::down_held())
    {
        _move_dy += k_speed;
        _direction = FacingDirection::Down;
    }

    _moving = (_move_dx != 0 || _move_dy != 0);

    // Normalize diagonal speed a bit
    if(_move_dx != 0 && _move_dy != 0)
    {
        _move_dx *= bn::fixed(0.707);
        _move_dy *= bn::fixed(0.707);
    }
}

// ---------------------------------------------------------------------------
// Collision-aware movement (wider hitbox)
// ---------------------------------------------------------------------------

void Player::_apply_movement(const WorldMap& world_map)
{
    bn::fixed_point new_pos = _pos;

    // Tune these if needed:
    constexpr bn::fixed feet_y_offset = 6;   // how far below center the "feet" are
    constexpr bn::fixed half_width    = 6;   // half-width of collision box

    auto can_stand_at = [&](const bn::fixed_point& base_pos)
    {
        // Base "feet" point:
        bn::fixed_point feet_center = base_pos;
        feet_center.set_y(feet_center.y() + 2 + feet_y_offset);

        // Left and right of the feet:
        bn::fixed_point feet_left  = feet_center;
        bn::fixed_point feet_right = feet_center;

        feet_left.set_x(feet_left.x() - half_width);
        feet_right.set_x(feet_right.x() + half_width - 1);

        // All three must be non-solid:
        return !world_map.is_solid(feet_center) &&
               !world_map.is_solid(feet_left) &&
               !world_map.is_solid(feet_right);
    };

    // Try X axis
    if(_move_dx != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_x(test.x() + _move_dx);

        if(can_stand_at(test))
        {
            new_pos.set_x(test.x());
        }
    }

    // Try Y axis
    if(_move_dy != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_y(test.y() + _move_dy);

        if(can_stand_at(test))
        {
            new_pos.set_y(test.y());
        }
    }

    _pos = new_pos;
}

// ---------------------------------------------------------------------------

void Player::_rebuild_sprites()
{
    // Pick sprite_items based on indices
    _body_item   = k_body_type_options[0];
    _hair_item   = k_hair_options[_appearance.hair_index];
    _eyes_item   = k_eyes_options[_appearance.eyes_index];
    _top_item    = k_top_options[_appearance.top_index];
    _bottom_item = k_bottom_options[_appearance.bottom_index];

    // Create sprites at current position
    _body_sprite   = _body_item->create_sprite(_pos);
    _eyes_sprite   = _eyes_item->create_sprite(_pos);
    _top_sprite    = _top_item->create_sprite(_pos);
    _bottom_sprite = _bottom_item->create_sprite(_pos);
    _hair_sprite   = _hair_item->create_sprite(_pos);

    // Attach camera if it already exists
    if(_camera)
    {
        auto& cam = _camera.value();
        _body_sprite->set_camera(cam);
        _eyes_sprite->set_camera(cam);
        _top_sprite->set_camera(cam);
        _bottom_sprite->set_camera(cam);
        _hair_sprite->set_camera(cam);
    }

    // Set z-order (body -> eyes -> hair -> bottom -> top)
    _body_sprite->set_z_order(4);
    _eyes_sprite->set_z_order(3);
    _hair_sprite->set_z_order(2);
    _bottom_sprite->set_z_order(1);
    _top_sprite->set_z_order(0);
}

// ---------------------------------------------------------------------------

void Player::_apply_colors()
{
    // Body (skin)
    {
        bn::sprite_palette_ptr pal = _body_sprite->palette();
        const ColorRamp& ramp = get_skin_ramp(_appearance.body_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Hair
    {
        bn::sprite_palette_ptr pal = _hair_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.hair_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Eyes
    {
        bn::sprite_palette_ptr pal = _eyes_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.eyes_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Top
    {
        bn::sprite_palette_ptr pal = _top_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.top_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Bottom
    {
        bn::sprite_palette_ptr pal = _bottom_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.bottom_color_index);
        apply_ramp_to_palette(ramp, pal);
    }
}

// ---------------------------------------------------------------------------

void Player::_update_animation()
{
    // Layout per direction:
    // [idle0, idle1, walk0, walk1, walk2, walk3]
    constexpr int k_idle_period = 24;  // frames between flips when idle
    constexpr int k_walk_period = 6;   // frames between steps when walking

    ++_anim_counter;

    if(_moving)
    {
        if(_anim_counter >= k_walk_period)
        {
            _anim_counter = 0;
            _walk_frame = (_walk_frame + 1) % 4;   // 0..3
        }
    }
    else
    {
        if(_anim_counter >= k_idle_period)
        {
            _anim_counter = 0;
            _idle_frame ^= 1;                      // toggle 0 <-> 1
        }

        // Reset walk cycle when you stop moving
        _walk_frame = 0;
    }
}

// ---------------------------------------------------------------------------

void Player::_sync_sprites()
{
    if(!_body_sprite || !_eyes_sprite || !_top_sprite || !_bottom_sprite || !_hair_sprite)
    {
        return; // safety
    }

    // Position
    _body_sprite->set_position(_pos);
    _eyes_sprite->set_position(_pos);
    _top_sprite->set_position(_pos);
    _bottom_sprite->set_position(_pos);
    _hair_sprite->set_position(_pos);

    _body_sprite->set_bg_priority(0);
    _eyes_sprite->set_bg_priority(0);
    _top_sprite->set_bg_priority(0);
    _bottom_sprite->set_bg_priority(0);
    _hair_sprite->set_bg_priority(0);

    // Apply camera if present
    if(_camera)
    {
        auto& cam = _camera.value();
        _body_sprite->set_camera(cam);
        _eyes_sprite->set_camera(cam);
        _top_sprite->set_camera(cam);
        _bottom_sprite->set_camera(cam);
        _hair_sprite->set_camera(cam);
    }

    // Frames per direction:
    // [idle0, idle1, walk0, walk1, walk2, walk3]
    constexpr int k_frames_per_direction = 6;

    // 0=Down, 1=Right, 2=Up, 3=Left (must match your FacingDirection)
    int dir = static_cast<int>(_direction);
    int base_frame = dir * k_frames_per_direction;

    int frame_index;
    if(_moving)
    {
        frame_index = base_frame + 2 + _walk_frame;   // walk0..3
    }
    else
    {
        frame_index = base_frame + _idle_frame;       // idle0..1
    }

    _body_sprite->set_tiles(_body_item->tiles_item(), frame_index);
    _eyes_sprite->set_tiles(_eyes_item->tiles_item(), frame_index);
    _hair_sprite->set_tiles(_hair_item->tiles_item(), frame_index);
    _bottom_sprite->set_tiles(_bottom_item->tiles_item(), frame_index);
    _top_sprite->set_tiles(_top_item->tiles_item(), frame_index);
}

// ---------------------------------------------------------------------------

void Player::_update_camera(const WorldMap& world_map)
{
    if(!_camera)
        return;

    int map_px_w = world_map.pixel_width();
    int map_px_h = world_map.pixel_height();

    const bn::fixed half_w = 120;   // 240 / 2
    const bn::fixed half_h = 80;    // 160 / 2

    bn::fixed cx = _pos.x();
    bn::fixed cy = _pos.y();

    bn::fixed min_x = bn::fixed(-map_px_w / 2) + half_w;
    bn::fixed max_x = bn::fixed( map_px_w / 2) - half_w;
    bn::fixed min_y = bn::fixed(-map_px_h / 2) + half_h;
    bn::fixed max_y = bn::fixed( map_px_h / 2) - half_h;

    if(min_x > max_x)
        cx = 0;
    else
        cx = bn::clamp(cx, min_x, max_x);

    if(min_y > max_y)
        cy = 0;
    else
        cy = bn::clamp(cy, min_y, max_y);

    _camera->set_x(cx);
    _camera->set_y(cy);
}

// ---------------------------------------------------------------------------

void Player::update(const WorldMap& world_map)
{
    _handle_input();
    _apply_movement(world_map);
    _update_animation();
    _update_camera(world_map);
    _sync_sprites();
}
