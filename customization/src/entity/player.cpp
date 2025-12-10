#include "player.h"

#include "bn_keypad.h"
#include "bn_math.h"

#include "world_map.h"

Player::Player(PlayerSprite* sprite,
               const bn::fixed_point& start_pos,
               const WorldMap* world) :
    Entity(
        sprite, world, 100, 1, Hitbox(0, 0, 6, 6), Hitbox(0, 0, 6, 6), 60
    ),
    _direction(FacingDirection::Down),
    _sprite(sprite)
{
    _sprite->rebuild(start_pos);
}

void Player::attach_camera(const bn::camera_ptr& camera)
{
    _camera = camera;
    _sprite->attach_camera(camera);
    Entity::attach_camera(camera);
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

    if(bn::keypad::a_pressed())
    {
        _sprite->play_attack();
    } 

    _moving = (_move_dx != 0 || _move_dy != 0);

    // Normalize diagonal speed a bit
    if(_move_dx != 0 && _move_dy != 0)
    {
        _move_dx *= bn::fixed(0.707);
        _move_dy *= bn::fixed(0.707);
    }
}

bn::fixed_point Player::_get_feet_position(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const {
    bn::fixed_point feet_pos = new_pos;
    bn::fixed feet_y_offset = 9;
    if((new_pos.y() - old_pos.y()) < 0)
    {
        feet_y_offset = 6;
    }
    feet_pos.set_y(feet_pos.y() + feet_y_offset);
    return feet_pos;
}

void Player::_apply_movement(bn::fixed_point& new_pos)
{
    // Try X axis
    if(_move_dx != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_x(test.x() + _move_dx);

        if(_can_stand_at(new_pos, test))
        {
            new_pos.set_x(test.x());
        }
    }

    // Try Y axis
    if(_move_dy != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_y(test.y() + _move_dy);

        if(_can_stand_at(new_pos, test))
        {
            new_pos.set_y(test.y());
        }
    }
}

void Player::_update_camera()
{
    if(!_camera)
        return;

    int map_px_w = _world_map->pixel_width();
    int map_px_h = _world_map->pixel_height();

    const bn::fixed half_w = 120;   // 240 / 2
    const bn::fixed half_h = 80;    // 160 / 2

    bn::fixed cx = _sprite->position().x();
    bn::fixed cy = _sprite->position().y();

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

void Player::update()
{
    bn::fixed_point new_pos = _sprite->position();

    if(_sprite->is_locked())
    {
        // No movement input while anim plays
        _move_dx = 0;
        _move_dy = 0;
        _moving = false;
    }
    else
    {
        _handle_input();
        _apply_movement(new_pos);
    }

    _sprite->update(new_pos, _direction, _moving);
    _update_camera();

    update_entity();
}
