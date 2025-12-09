#include "enemy.h"

#include "bn_math.h"
#include "world_map.h"
#include "bn_log.h"

namespace
{
    constexpr bn::fixed_point ZERO_VELOCITY(0, 0);
    constexpr bn::fixed       STOP_THRESHOLD(0.05);
}

Enemy::Enemy(EntitySprite* sprite, const WorldMap* world_map) :
    Entity(
        sprite, world_map, 5, 1, Hitbox(0, 0, 6, 6), Hitbox(0, 0, 6, 6)
    ),
    _direction(FacingDirection::Down),
    _velocity(ZERO_VELOCITY),
    _max_speed(0.7),
    _aggro_radius(64),
    _lose_radius(128),
    _attack_cooldown(0),
    _attack_cooldown_max(60),
    _path_recalc_timer(0)
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

    update_entity();
}

bn::fixed_point Enemy::_get_feet_position(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const {
    bn::fixed_point feet_pos = new_pos;
    bn::fixed feet_y_offset = 9;
    if((new_pos.y() - old_pos.y()) < 0)
    {
        feet_y_offset = 6;
    }
    feet_pos.set_y(feet_pos.y() + feet_y_offset);
    return feet_pos;
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
        _update_path_and_velocity(my_pos, target_pos);
    }
    else if(dist_sq >= lose_sq)
    {
        // Too far; forget and stop moving
        _velocity = ZERO_VELOCITY;
    }
    else
    {
        // In "chase" band; keep trying to move toward target
        _update_path_and_velocity(my_pos, target_pos);
    }
}

// -----------------------------------------------------------------------------
// Simple local "pathfinding"
// -----------------------------------------------------------------------------
// Instead of just going straight toward the player, we test the 4 cardinal
// directions a small step ahead, skip blocked ones, and pick the one that
// gets us closest to the player. This lets enemies slide around simple
// obstacles while staying cheap for the GBA.
// -----------------------------------------------------------------------------
void Enemy::_update_path_and_velocity(const bn::fixed_point& my_pos,
                                      const bn::fixed_point& target_pos)
{
    // If we don't have a world map, just fall back to the old direct chase.
    if(!_world_map)
    {
        _chase(bn::fixed_point(
            target_pos.x() - my_pos.x(),
            target_pos.y() - my_pos.y()
        ));
        return;
    }

    if(_path_recalc_timer > 0)
    {
        --_path_recalc_timer;
        // Keep current velocity for now.
        return;
    }

    _path_recalc_timer = k_path_recalc_interval;

    // Candidate directions: stand still, left, right, up, down
    // (no diagonals to keep speed consistent and code simple).
    constexpr int DIR_COUNT = 5;
    const bn::fixed_point directions[DIR_COUNT] = {
        bn::fixed_point(0,  0),   // stay still
        bn::fixed_point(1,  0),   // right
        bn::fixed_point(-1, 0),   // left
        bn::fixed_point(0,  1),   // down
        bn::fixed_point(0, -1)    // up
    };

    constexpr bn::fixed step = 8;     // "look" 8 pixels ahead

    bn::fixed       best_dist_sq = bn::fixed(32767);
    bn::fixed_point best_dir(0, 0);

    for(int i = 1; i < DIR_COUNT; ++i)   // skip index 0 (stand still) as candidate
    {
        const bn::fixed_point dir = directions[i];

        bn::fixed_point candidate_pos(
            my_pos.x() + dir.x() * step,
            my_pos.y() + dir.y() * step
        );

        if(!_can_stand_at(my_pos, candidate_pos))
        {
            continue;
        }

        const bn::fixed cdx = target_pos.x() - candidate_pos.x();
        const bn::fixed cdy = target_pos.y() - candidate_pos.y();
        const bn::fixed d2  = cdx * cdx + cdy * cdy;

        if(d2 < best_dist_sq)
        {
            BN_LOG(d2, best_dist_sq);
            best_dist_sq = d2;
            best_dir     = dir;
        }
    }

    _velocity.set_x(best_dir.x() * _max_speed);
    _velocity.set_y(best_dir.y() * _max_speed);
}

// Fallback: straight-line chase (used only if no world_map).
void Enemy::_chase(const bn::fixed_point& to_target)
{
    const bn::fixed tx = to_target.x();
    const bn::fixed ty = to_target.y();

    if(tx == 0 && ty == 0)
    {
        _velocity = ZERO_VELOCITY;
        return;
    }

    // Normalize using max component (same as your original code).
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

// -----------------------------------------------------------------------------
// Movement + animation (now with collisions)
// -----------------------------------------------------------------------------
void Enemy::_apply_movement_animation()
{
    bn::fixed_point new_pos = position();

    // Apply axis-separated movement with collision checks (same pattern as Player).
    if(_world_map)
    {
        // X axis
        if(_velocity.x() != 0)
        {
            bn::fixed_point test = new_pos;
            test.set_x(test.x() + _velocity.x());

            if(_can_stand_at(new_pos, test))
            {
                new_pos.set_x(test.x());
            }
            else
            {
                _velocity.set_x(0);
            }
        }

        // Y axis
        if(_velocity.y() != 0)
        {
            bn::fixed_point test = new_pos;
            test.set_y(test.y() + _velocity.y());

            if(_can_stand_at(new_pos, test))
            {
                new_pos.set_y(test.y());
            }
            else
            {
                _velocity.set_y(0);
            }
        }
    }
    else
    {
        // No world map: just move freely.
        new_pos.set_x(new_pos.x() + _velocity.x());
        new_pos.set_y(new_pos.y() + _velocity.y());
    }

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

    // This assumes your sprite.update() stores the new position internally so that
    // Entity::position() keeps in sync, just like your Player does.
    _sprite->update(new_pos, _direction, moving);
}
