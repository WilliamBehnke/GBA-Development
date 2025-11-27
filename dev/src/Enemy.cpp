#include "Enemy.h"

#include "bn_math.h"      // for bn::abs, bn::max

Enemy::Enemy(BaseSprite* sprite) :
    Entity(
        sprite,
        10,                   // max health
        Hitbox(0, 0, 8, 8),    // hurt box
        Hitbox(0, 0, 9, 9)     // attack box
    ),
    _velocity(0, 0),
    _max_speed(0.7),
    _accel(0.15),              // smoothing factor
    _aggro_radius(50),         // start chasing
    _lose_radius(100),          // give up chase
    _attack_cooldown_max(0)   // ~0.75s at 60fps
{}

void Enemy::add_patrol_point(const bn::fixed_point& point)
{
    if(_patrol_points.size() < max_patrol_points)
    {
        _patrol_points.push_back(point);
    }
}

void Enemy::clear_patrol_points()
{
    _patrol_points.clear();
    _current_patrol_index = 0;
}

void Enemy::update()
{
    if(!_sprite)
    {
        Entity::update();
        return;
    }

    if(_sprite->is_locked())
    {
        _velocity = bn::fixed_point(0, 0);
        Entity::update();
        return;
    }

    if(_attack_cooldown > 0)
    {
        --_attack_cooldown;
    }

    _update_ai();
    _apply_movement_animation();

    Entity::update();
}

// ------------------------------------------------------
// AI: decide between patrol / chase / attack
// ------------------------------------------------------
void Enemy::_update_ai()
{
    if(_target && _target->is_alive())
    {
        bn::fixed_point my_pos     = position();
        bn::fixed_point target_pos = _target->position();

        bn::fixed_point to_target(
            target_pos.x() - my_pos.x(),
            target_pos.y() - my_pos.y()
        );

        bn::fixed dx = to_target.x();
        bn::fixed dy = to_target.y();
        bn::fixed dist_sq = dx * dx + dy * dy;

        bn::fixed aggro_sq  = _aggro_radius  * _aggro_radius;
        bn::fixed lose_sq   = _lose_radius   * _lose_radius;

        // ----------------------------------------------
        // ATTACK: only if cooldown ready AND hitbox
        // overlap says the attack would actually land
        // ----------------------------------------------
        if(_attack_cooldown <= 0 && attack_hits(*_target))
        {
            // Stop moving to attack
            _velocity = bn::fixed_point(0, 0);

            // Directional attacks could use facing dir later
            _sprite->play_attack();

            _attack_cooldown = _attack_cooldown_max;
            return;
        }

        // ----------------------------------------------
        // CHASE / PATROL logic
        // ----------------------------------------------

        // In aggro range: chase
        if(dist_sq <= aggro_sq)
        {
            _chase(to_target);
            return;
        }

        // Too far: go back to patrolling
        if(dist_sq >= lose_sq)
        {
            _patrol();
            return;
        }

        // Between aggro and lose: keep chasing for smoother feel
        _chase(to_target);
    }
    else
    {
        // No target: just patrol
        _patrol();
    }
}

// ------------------------------------------------------
// Patrol: follow a list of waypoints, looping
// ------------------------------------------------------
void Enemy::_patrol()
{
    if(_patrol_points.empty())
    {
        // No patrol points: slow to a stop and idle
        _velocity = bn::fixed_point(
            _velocity.x() + (bn::fixed(0) - _velocity.x()) * _accel,
            _velocity.y() + (bn::fixed(0) - _velocity.y()) * _accel
        );
        return;
    }

    // Current target waypoint
    const bn::fixed_point& target = _patrol_points[_current_patrol_index];
    bn::fixed_point pos = position();

    bn::fixed dx = target.x() - pos.x();
    bn::fixed dy = target.y() - pos.y();

    // Have we "reached" this waypoint?
    const bn::fixed reach_radius = 2;   // pixels
    bn::fixed dist_sq = dx * dx + dy * dy;
    if(dist_sq <= reach_radius * reach_radius)
    {
        // Advance to next waypoint
        ++_current_patrol_index;
        if(_current_patrol_index >= _patrol_points.size())
        {
            if(_loop_patrol && !_patrol_points.empty())
                _current_patrol_index = 0;
            else
                _current_patrol_index = _patrol_points.size() - 1;
        }
    }

    // Re-fetch target in case index changed
    const bn::fixed_point& new_target = _patrol_points[_current_patrol_index];
    bn::fixed ndx = new_target.x() - pos.x();
    bn::fixed ndy = new_target.y() - pos.y();

    // Desired velocity toward this waypoint
    bn::fixed abs_x = bn::abs(ndx);
    bn::fixed abs_y = bn::abs(ndy);
    bn::fixed max_comp = bn::max(abs_x, abs_y);

    bn::fixed desired_x = 0;
    bn::fixed desired_y = 0;

    if(max_comp > 0)
    {
        bn::fixed nx = ndx / max_comp;
        bn::fixed ny = ndy / max_comp;

        desired_x = nx * _max_speed;
        desired_y = ny * _max_speed;
    }

    // Smooth acceleration toward desired velocity
    _velocity.set_x(_velocity.x() + (desired_x - _velocity.x()) * _accel);
    _velocity.set_y(_velocity.y() + (desired_y - _velocity.y()) * _accel);
}

// ------------------------------------------------------
// Chase: move toward player with smoothed acceleration
// ------------------------------------------------------
void Enemy::_chase(const bn::fixed_point& to_target)
{
    bn::fixed abs_x = bn::abs(to_target.x());
    bn::fixed abs_y = bn::abs(to_target.y());
    bn::fixed max_comp = bn::max(abs_x, abs_y);

    bn::fixed desired_x = 0;
    bn::fixed desired_y = 0;

    if(max_comp > 0)
    {
        bn::fixed nx = to_target.x() / max_comp;
        bn::fixed ny = to_target.y() / max_comp;

        desired_x = nx * _max_speed;
        desired_y = ny * _max_speed;
    }

    _velocity.set_x(_velocity.x() + (desired_x - _velocity.x()) * _accel);
    _velocity.set_y(_velocity.y() + (desired_y - _velocity.y()) * _accel);
}

// ------------------------------------------------------
// Apply movement + animations based on velocity
// ------------------------------------------------------
void Enemy::_apply_movement_animation()
{
    if(_sprite->is_locked())
    {
        Entity::update();
        return;
    }

    const bn::fixed stop_threshold = bn::fixed(0.05);

    bool moving =
        bn::abs(_velocity.x()) > stop_threshold ||
        bn::abs(_velocity.y()) > stop_threshold;

    if(!moving)
    {
        _velocity = bn::fixed_point(0, 0);

        // If your idle also takes a direction, do:
        //   _sprite->play_idle(_facing_dir);
        // For now I'll assume parameter-less idle:
        _sprite->play_idle();
    }
    else
    {
        // Decide facing direction based on velocity
        direction dir = direction::DOWN;   // default

        bn::fixed abs_x = bn::abs(_velocity.x());
        bn::fixed abs_y = bn::abs(_velocity.y());

        if(abs_x >= abs_y)
        {
            // Horizontal dominates
            if(_velocity.x() < 0)
            {
                dir = direction::LEFT;
            }
            else
            {
                dir = direction::RIGHT;
            }
        }
        else
        {
            // Vertical dominates
            if(_velocity.y() < 0)
            {
                dir = direction::UP;
            }
            else
            {
                dir = direction::DOWN;
            }
        }

        // Play walk in that direction
        _sprite->play_walk(dir);
    }

    // Actually move the enemy in the world
    move_by(_velocity);

    Entity::update();
}
