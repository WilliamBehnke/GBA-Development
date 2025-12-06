#include "player.h"

#include "bn_keypad.h"
#include "bn_math.h"

#include "world_map.h"

Player::Player(const CharacterAppearance& appearance,
               const bn::fixed_point& start_pos) :
    _appearance(appearance),
    _pos(start_pos),
    _direction(_appearance.direction),
    _sprite(_appearance)
{
    _sprite.rebuild(_pos);
}

void Player::attach_camera(const bn::camera_ptr& camera)
{
    _camera = camera;
    _sprite.attach_camera(camera);
}

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

void Player::_apply_movement(const WorldMap* world_map)
{
    bn::fixed_point new_pos = _pos;

    // Tune these if needed:
    bn::fixed feet_y_offset = 9;
    if(_move_dy < 0)
    {
        feet_y_offset = 6;
    }
    constexpr bn::fixed half_width = 6;   // half-width of collision box

    auto can_stand_at = [&](const bn::fixed_point& base_pos)
    {
        // Base "feet" point:
        bn::fixed_point feet_center = base_pos;
        feet_center.set_y(feet_center.y() + feet_y_offset);

        // Left and right of the feet:
        bn::fixed_point feet_left  = feet_center;
        bn::fixed_point feet_right = feet_center;

        feet_left.set_x(feet_left.x() - half_width);
        feet_right.set_x(feet_right.x() + half_width - 1);

        // All three must be non-solid:
        return !world_map->is_solid(feet_center) &&
               !world_map->is_solid(feet_left) &&
               !world_map->is_solid(feet_right);
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

void Player::_update_camera(const WorldMap* world_map)
{
    if(!_camera)
        return;

    int map_px_w = world_map->pixel_width();
    int map_px_h = world_map->pixel_height();

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

void Player::update_sprite()
{
    _sprite.update(_pos, _direction, _moving);
}

void Player::update(const WorldMap* world_map)
{
    _handle_input();
    _apply_movement(world_map);
    _update_camera(world_map);

    _sprite.update(_pos, _direction, _moving);
}
