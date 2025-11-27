#include "Player.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_fixed_point.h"

namespace {
    constexpr bn::fixed k_speed = 1.5;
    constexpr bn::fixed k_half_width  = 120;
    constexpr bn::fixed k_half_height = 80;

    void clamp_to_bounds(bn::fixed_point& pos) {
        if(pos.x() < -k_half_width) {
            pos.set_x(-k_half_width);
        } else if(pos.x() > k_half_width) {
            pos.set_x(k_half_width);
        }

        if(pos.y() < -k_half_height) {
            pos.set_y(-k_half_height);
        } else if(pos.y() > k_half_height) {
            pos.set_y(k_half_height);
        }
    }
}

Player::Player(BaseSprite* sprite) :
    Entity(
        sprite, 5, 2, Hitbox(0, 0, 8, 8), Hitbox(0, 0, 8, 8)
    ),
    _facing(direction::DOWN)
{}

void Player::update() {
    if(_sprite->is_locked()) {
        Entity::update();
        return;
    }

    bn::fixed dx = 0;
    bn::fixed dy = 0;
    bool moving = false;

    if(bn::keypad::left_held()) {
        dx -= k_speed;
        moving = true;
        _facing = direction::LEFT;
    }
    if(bn::keypad::right_held()) {
        dx += k_speed;
        moving = true;
        _facing = direction::RIGHT;
    }
    if(bn::keypad::up_held()) {
        dy -= k_speed;
        moving = true;
        _facing = direction::UP;
    }
    if(bn::keypad::down_held()) {
        dy += k_speed;
        moving = true;
        _facing = direction::DOWN;
    }

    // Normalize diagonal
    if(moving && (dx != 0 || dy != 0)) {
        const bn::fixed len = bn::sqrt(dx * dx + dy * dy);
        if(len != 0) {
            dx = (dx / len) * k_speed;
            dy = (dy / len) * k_speed;
        }
    }

    bn::fixed_point pos = _sprite->sprite().position();
    pos.set_x(pos.x() + dx);
    pos.set_y(pos.y() + dy);
    clamp_to_bounds(pos);
    _sprite->sprite().set_position(pos);

    if(bn::keypad::a_pressed()) {
        _sprite->play_attack();
    } else if(moving) {
        _sprite->play_walk(_facing);
    } else {
        _sprite->play_idle();
    }

    // Update attack hitbox offset based on facing
    switch(_facing)
    {
        case direction::DOWN:
            _attack_box.offset_x = 0;
            _attack_box.offset_y = 8;
            break;
        case direction::UP:
            _attack_box.offset_x = 0;
            _attack_box.offset_y = -8;
            break;
        case direction::LEFT:
            _attack_box.offset_x = -8;
            _attack_box.offset_y = 0;
            break;
        case direction::RIGHT:
            _attack_box.offset_x = 8;
            _attack_box.offset_y = 0;
            break;
    }

    Entity::update();
}
