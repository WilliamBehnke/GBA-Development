#include "Enemy.h"

#include "bn_math.h"

namespace {
    constexpr bn::fixed_point ZERO_VELOCITY(0, 0);
    constexpr bn::fixed       STOP_THRESHOLD(0.05);
}

Enemy::Enemy(BaseSprite* sprite) :
    Entity(
        sprite, 10, 1, Hitbox(0, 0, 8, 8), Hitbox(0, 0, 8, 8)
    ),
    _velocity(ZERO_VELOCITY),
    _max_speed(0.7),
    _aggro_radius(50),
    _lose_radius(100),
    _attack_cooldown(0),
    _attack_cooldown_max(30)
{}

void Enemy::update() {
    if(!_sprite) {
        Entity::update();
        return;
    }

    if(_sprite->is_locked()) {
        _velocity = ZERO_VELOCITY;
        Entity::update();
        return;
    }

    if(_attack_cooldown > 0) {
        --_attack_cooldown;
    }

    _update_ai();
    _apply_movement_animation();

    Entity::update();
}

void Enemy::_update_ai() {
    _start_attack = false;

    if(!_target || !_target->is_alive()) {
        _velocity = ZERO_VELOCITY;
        return;
    }

    const bn::fixed_point my_pos     = position();
    const bn::fixed_point target_pos = _target->position();
    const bn::fixed_point to_target(
        target_pos.x() - my_pos.x(),
        target_pos.y() - my_pos.y()
    );

    const bn::fixed dx       = to_target.x();
    const bn::fixed dy       = to_target.y();
    const bn::fixed dist_sq  = dx * dx + dy * dy;
    const bn::fixed aggro_sq = _aggro_radius * _aggro_radius;
    const bn::fixed lose_sq  = _lose_radius  * _lose_radius;

    // Try to attack if in range and off cooldown
    if(_attack_cooldown <= 0 && attack_hits(*_target)) {
        _velocity = ZERO_VELOCITY;
        _start_attack = true;
        _attack_cooldown = _attack_cooldown_max;
        return;
    }

    // Movement logic based on distance
    if(dist_sq <= aggro_sq) {
        _chase(to_target);
    } else if(dist_sq >= lose_sq) {
        _velocity = ZERO_VELOCITY;
    } else {
        _chase(to_target);
    }
}

void Enemy::_chase(const bn::fixed_point& to_target) {
    const bn::fixed tx = to_target.x();
    const bn::fixed ty = to_target.y();

    if(tx == 0 && ty == 0) {
        _velocity = ZERO_VELOCITY;
        return;
    }

    // Normalize using max component
    const bn::fixed abs_x    = bn::abs(tx);
    const bn::fixed abs_y    = bn::abs(ty);
    const bn::fixed max_comp = bn::max(abs_x, abs_y);

    if(max_comp <= 0) {
        _velocity = ZERO_VELOCITY;
        return;
    }

    const bn::fixed nx = tx / max_comp;
    const bn::fixed ny = ty / max_comp;

    _velocity.set_x(nx * _max_speed);
    _velocity.set_y(ny * _max_speed);
}

void Enemy::_apply_movement_animation() {
    const bool moving =
        bn::abs(_velocity.x()) > STOP_THRESHOLD ||
        bn::abs(_velocity.y()) > STOP_THRESHOLD;

    if(_start_attack) {
        _sprite->play_attack();
    } else if(!moving) {
        _velocity = ZERO_VELOCITY;
        _sprite->play_idle();
    } else {
        // Decide facing direction based on velocity
        direction dir = direction::DOWN;

        const bn::fixed abs_x = bn::abs(_velocity.x());
        const bn::fixed abs_y = bn::abs(_velocity.y());

        if(abs_x >= abs_y) {
            dir = (_velocity.x() < 0) ? direction::LEFT : direction::RIGHT;
        } else {
            dir = (_velocity.y() < 0) ? direction::UP : direction::DOWN;
        }

        _sprite->play_walk(dir);
    }

    // Actually move the enemy in the world
    move_by(_velocity);
}
