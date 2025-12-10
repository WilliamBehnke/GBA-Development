#include "player.h"

#include "bn_keypad.h"
#include "bn_math.h"

#include "world_map.h"

namespace
{
    constexpr bn::fixed k_block_speed_factor = 0.25;    // drastically reduced speed
}

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

    // Previous blocking state (for edge detection)
    bool was_blocking = _is_blocking;

    // -----------------------------------------------------------------------
    // Blocking input: hold R to block
    // -----------------------------------------------------------------------
    _is_blocking = bn::keypad::r_held();

    // If we just released R, immediately leave block state on the sprite
    if(bn::keypad::r_released())
    {
        _sprite->play_idle();
    }

    // -----------------------------------------------------------------------
    // Movement input
    // -----------------------------------------------------------------------
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

    // If blocking, drastically reduce speed
    if(_is_blocking)
    {
        _move_dx *= k_block_speed_factor;
        _move_dy *= k_block_speed_factor;

        // While blocking, we prioritize the block animation
        if(!was_blocking)
        {
            _sprite->play_block();
        }
    }

    // -----------------------------------------------------------------------
    // Attack / hurt / death input
    //   - Disable these while blocking so block has priority
    // -----------------------------------------------------------------------
    if(!_is_blocking)
    {
        if(bn::keypad::a_pressed())
        {
            _sprite->play_attack();
        }
        else if(bn::keypad::b_pressed())
        {
            _sprite->play_hurt();
        }
        else if(bn::keypad::select_pressed())
        {
            _sprite->play_death();
        }
    }

    _moving = (_move_dx != 0 || _move_dy != 0);

    // Normalize diagonal speed a bit
    if(_move_dx != 0 && _move_dy != 0)
    {
        _move_dx *= bn::fixed(0.707);
        _move_dy *= bn::fixed(0.707);
    }
}

bn::fixed_point Player::_get_feet_position(const bn::fixed_point& old_pos,
                                           const bn::fixed_point& new_pos) const
{
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
    if(!_camera || !_world_map)
        return;

    // Clamp player to world bounds
    bn::fixed_point clamped_player = _clamp_to_world(_sprite->position());

    const int map_px_w = _world_map->pixel_width();
    const int map_px_h = _world_map->pixel_height();

    constexpr bn::fixed half_screen_w = 120;   // 240 / 2
    constexpr bn::fixed half_screen_h = 80;    // 160 / 2

    const bn::fixed world_half_w = bn::fixed(map_px_w / 2);
    const bn::fixed world_half_h = bn::fixed(map_px_h / 2);

    const bn::fixed min_cam_x = -world_half_w + half_screen_w;
    const bn::fixed max_cam_x =  world_half_w - half_screen_w;
    const bn::fixed min_cam_y = -world_half_h + half_screen_h;
    const bn::fixed max_cam_y =  world_half_h - half_screen_h;

    bn::fixed cx;
    bn::fixed cy;

    if(min_cam_x > max_cam_x)
        cx = 0;
    else
        cx = bn::clamp(clamped_player.x(), min_cam_x, max_cam_x);

    if(min_cam_y > max_cam_y)
        cy = 0;
    else
        cy = bn::clamp(clamped_player.y(), min_cam_y, max_cam_y);

    _camera->set_x(cx);
    _camera->set_y(cy);
}

void Player::update()
{
    bn::fixed_point new_pos = _sprite->position();

    if(_sprite->is_locked())
    {
        // No movement input while certain anims play (attack/hurt/death)
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

// ---------------------------------------------------------------------------
// Blocking hooks from Entity
// ---------------------------------------------------------------------------

// Called by Entity::take_damage(...) to see if this hit is fully blocked.
bool Player::is_blocking_attack_from(const bn::fixed_point& source_pos) const
{
    if(!_is_blocking)
    {
        return false;
    }

    const bn::fixed_point pos = position();

    // Vector from player to attacker
    const bn::fixed dx = source_pos.x() - pos.x();
    const bn::fixed dy = source_pos.y() - pos.y();

    if(dx == 0 && dy == 0)
    {
        // Overlapping / same tile; treat as not blockable to avoid weird edge case
        return false;
    }

    const bn::fixed abs_x = bn::abs(dx);
    const bn::fixed abs_y = bn::abs(dy);

    // Determine attack direction as seen from the player
    FacingDirection attack_dir;
    if(abs_x >= abs_y)
    {
        attack_dir = (dx > 0) ? FacingDirection::Right : FacingDirection::Left;
    }
    else
    {
        attack_dir = (dy > 0) ? FacingDirection::Down : FacingDirection::Up;
    }

    // Blocking only works when facing the attacker
    const bool is_front = (attack_dir == _direction);

    if(is_front && _sprite)
    {
        // Trigger block-success frame when a hit is actually blocked
        _sprite->play_block_success();
    }

    return is_front;
}

// Called by Entity::take_damage(...) for hits that are NOT blocked.
void Player::on_block_broken()
{
    if(_is_blocking)
    {
        // Leave block stance
        _is_blocking = false;
    }
}
