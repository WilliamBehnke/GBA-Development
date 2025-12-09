#include "enemy.h"

#include "bn_math.h"

namespace
{
    constexpr bn::fixed_point ZERO_VELOCITY(0, 0);
    constexpr bn::fixed       STOP_THRESHOLD(0.05);
}

Enemy::Enemy(EntitySprite* sprite) :
    Entity(
        sprite, 5, 1, Hitbox(0, 0, 6, 6), Hitbox(0, 0, 6, 6)
    ),
    _velocity(ZERO_VELOCITY),
    _max_speed(0.7),
    _aggro_radius(50),
    _lose_radius(100),
    _attack_cooldown(0),
    _attack_cooldown_max(60)
{
}

void Enemy::attach_camera(const bn::camera_ptr& camera)
{
    _sprite->attach_camera(camera);
    Entity::attach_camera(camera);
}

void Enemy::update()
{
    _update_ai();
    _apply_movement_animation();

    Entity::update();
}

void Enemy::_update_ai()
{
    _start_attack = false;

    if(!is_alive() || !_target || !_target->is_alive())
    {
        _velocity = ZERO_VELOCITY;
        return;
    }

    if(_attack_cooldown > 0) 
    {
        --_attack_cooldown;
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
    if(!_sprite->is_locked() && _attack_cooldown <= 0 && attack_hits(*_target))
    {
        _velocity = ZERO_VELOCITY;
        _start_attack = true;
        _attack_cooldown = _attack_cooldown_max;
        return;
    }

    // Movement logic based on distance
    if(dist_sq <= aggro_sq)
    {
        _chase(to_target);
    }
    else if(dist_sq >= lose_sq)
    {
        _velocity = ZERO_VELOCITY;
    }
    else
    {
        _chase(to_target);
    }
}

void Enemy::_chase(const bn::fixed_point& to_target)
{
    const bn::fixed tx = to_target.x();
    const bn::fixed ty = to_target.y();

    if(tx == 0 && ty == 0)
    {
        _velocity = ZERO_VELOCITY;
        return;
    }

    // Normalize using max component
    const bn::fixed abs_x    = bn::abs(tx);
    const bn::fixed abs_y    = bn::abs(ty);
    const bn::fixed max_comp = bn::max(abs_x, abs_y);

    if(max_comp <= 0)
    {
        _velocity = ZERO_VELOCITY;
        return;
    }

    const bn::fixed nx = tx / max_comp;
    const bn::fixed ny = ty / max_comp;

    _velocity.set_x(nx * _max_speed);
    _velocity.set_y(ny * _max_speed);
}

void Enemy::_apply_movement_animation()
{
    const bool moving =
        bn::abs(_velocity.x()) > STOP_THRESHOLD ||
        bn::abs(_velocity.y()) > STOP_THRESHOLD;

    if(_start_attack)
    {
        _sprite->play_attack();
    }
    else if(!moving)
    {
        _velocity = ZERO_VELOCITY;
    }
    else
    {
        const bn::fixed abs_x = bn::abs(_velocity.x());
        const bn::fixed abs_y = bn::abs(_velocity.y());

        if(abs_x >= abs_y)
        {
            _direction = (_velocity.x() < 0) ? FacingDirection::Left : FacingDirection::Right;
        }
        else
        {
            _direction = (_velocity.y() < 0) ? FacingDirection::Up : FacingDirection::Down;
        }
    }

    _sprite->update(position() + _velocity, _direction, moving);
}
