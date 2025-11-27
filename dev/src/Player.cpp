#include "Player.h"

#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_log.h"

namespace
{
    constexpr bn::fixed k_speed = 1.5;
    constexpr bn::fixed k_half_width  = 120;   // 240 / 2
    constexpr bn::fixed k_half_height = 80;    // 160 / 2

    void clamp_to_bounds(bn::fixed_point& pos)
    {
        if(pos.x() < -k_half_width)
        {
            pos.set_x(-k_half_width);
        }
        else if(pos.x() > k_half_width)
        {
            pos.set_x(k_half_width);
        }

        if(pos.y() < -k_half_height)
        {
            pos.set_y(-k_half_height);
        }
        else if(pos.y() > k_half_height)
        {
            pos.set_y(k_half_height);
        }
    }
}

Player::Player(CharacterSprite& sprite) :
    _sprite(sprite),
    _facing(direction::DOWN)
{
}

void Player::update()
{
    if(_sprite.is_attacking() || _sprite.is_hurt()) {
        _sprite.update();
        return;
    }

    if(bn::keypad::a_pressed())
    {
        _sprite.start_attack(_facing);
        return;
    }
    
    if(bn::keypad::b_pressed())
    {
        _sprite.start_hurt(_facing);
        return;
    }

    bn::fixed dx = 0;
    bn::fixed dy = 0;
    bool moving = false;

    // -----------------------------------------------------
    // MOVEMENT INPUT
    // -----------------------------------------------------
    if(bn::keypad::left_held())
    {
        dx -= k_speed;
        moving = true;
        _facing = direction::LEFT;
    }
    if(bn::keypad::right_held())
    {
        dx += k_speed;
        moving = true;
        _facing = direction::RIGHT;
    }
    if(bn::keypad::up_held())
    {
        dy -= k_speed;
        moving = true;
        _facing = direction::UP;
    }
    if(bn::keypad::down_held())
    {
        dy += k_speed;
        moving = true;
        _facing = direction::DOWN;
    }

    // Normalize diagonal movement
    if(moving && (dx != 0 || dy != 0))
    {
        const bn::fixed inv_len = bn::fixed(1) / bn::sqrt(dx * dx + dy * dy);
        dx *= inv_len * k_speed;
        dy *= inv_len * k_speed;
    }

    bn::fixed_point pos = _sprite.position();
    pos.set_x(pos.x() + dx);
    pos.set_y(pos.y() + dy);

    clamp_to_bounds(pos);
    _sprite.set_position(pos);

    if(moving)
    {
        _sprite.set_walk(_facing);
    }
    else
    {
        _sprite.set_idle(_facing);
    }

    // -----------------------------------------------------
    // UPDATE ANIMATIONS
    // -----------------------------------------------------
    _sprite.update();
}
