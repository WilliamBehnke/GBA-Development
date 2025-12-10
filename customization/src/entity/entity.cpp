#include "entity.h"
#include "world_map.h"

Entity::Entity(EntitySprite* sprite,
               const WorldMap* world_map,
               int max_health,
               int damage,
               const Hitbox& hurt_box,
               const Hitbox& attack_box,
               int invuln_duration_frames) :
    _sprite(sprite),
    _world_map(world_map),
    _health(max_health),
    _max_health(max_health),
    _damage(damage),
    _hurt_box(hurt_box),
    _attack_box(attack_box),
    _invuln_duration(invuln_duration_frames)
{
}

void Entity::set_active(bool active)
{
    _active = active;

    if(!active) 
    {
        _health_bar.hide();
    }

    if(_sprite)
    {
        _sprite->set_visible(active);
    }
}

bn::fixed_point Entity::_clamp_to_world(const bn::fixed_point& candidate) const
{
    if(!_world_map)
    {
        return candidate;
    }

    const int map_px_w = _world_map->pixel_width();
    const int map_px_h = _world_map->pixel_height();

    // Same coordinate model as your camera: (0,0) is map center.
    const bn::fixed half_w = bn::fixed(map_px_w / 2);
    const bn::fixed half_h = bn::fixed(map_px_h / 2);

    const bn::fixed min_x = -half_w;
    const bn::fixed max_x =  half_w;
    const bn::fixed min_y = -half_h;
    const bn::fixed max_y =  half_h;

    const bn::fixed clamped_x = bn::clamp(candidate.x(), min_x, max_x);
    const bn::fixed clamped_y = bn::clamp(candidate.y(), min_y, max_y);

    return bn::fixed_point(clamped_x, clamped_y);
}

bool Entity::_can_stand_at(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const
{
    if(!_world_map)
    {
        return true;
    }

    bn::fixed_point feet_center = _get_feet_position(old_pos, new_pos);

    bn::fixed_point feet_left  = feet_center;
    bn::fixed_point feet_right = feet_center;

    feet_left.set_x(feet_left.x() - _hurt_box.half_width);
    feet_right.set_x(feet_right.x() + _hurt_box.half_width - 1);

    return !_world_map->is_solid(feet_center) &&
           !_world_map->is_solid(feet_left)   &&
           !_world_map->is_solid(feet_right);
}

void Entity::move_by(const bn::fixed_point& delta)
{
    if(!_sprite || !_world_map)
    {
        return;
    }

    bn::fixed_point new_pos = _sprite->position();

    // X axis
    if(delta.x() != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_x(test.x() + delta.x());

        if(_can_stand_at(new_pos, test))
        {
            new_pos.set_x(test.x());
        }
    }

    // Y axis
    if(delta.y() != 0)
    {
        bn::fixed_point test = new_pos;
        test.set_y(test.y() + delta.y());

        if(_can_stand_at(new_pos, test))
        {
            new_pos.set_y(test.y());
        }
    }

    // Clamp to map bounds so we never leave the world
    new_pos = _clamp_to_world(new_pos);

    _sprite->set_position(new_pos);
}

void Entity::attach_camera(const bn::camera_ptr& camera)
{
    _health_bar.attach_camera(camera);
}

void Entity::update_entity()
{
    _apply_knockback();

    if(_sprite)
    {
        // Y-sort: bigger Y = closer to camera (on top)
        if(is_alive())
        {
            bn::fixed_point pos = _sprite->position();
            int z = -pos.y().integer();
            _sprite->set_z_order(z);

            if(_show_health_bar)
            {
                _health_bar.update(
                    pos, _health, _max_health, z
                );
            }
        }
        else
        {
            _health_bar.hide();
        }
    }

    _tick_invulnerability();
}

bn::fixed_point Entity::position() const
{
    return _sprite ? _sprite->position() : bn::fixed_point();
}

bool Entity::is_attacking() const
{
    if(!_sprite)
    {
        return false;
    }
    return _sprite->animation_state() == EntitySprite::AnimationState::Attack;
}

void Entity::take_damage(int amount)
{
    // Fallback: no direction info (no meaningful knockback dir)
    take_damage(amount, position());
}

void Entity::take_damage(int amount, const bn::fixed_point& source_pos)
{
    if(amount <= 0 || !is_alive() || is_invulnerable())
    {
        return;
    }

    _health -= amount;
    if(_health < 0)
    {
        _health = 0;
    }

    _invuln_timer = _invuln_duration;

    if(_sprite)
    {
        if(_health <= 0)
        {
            _sprite->play_death();
        }
        else
        {
            _sprite->play_hurt();
        }
    }

    _start_knockback(source_pos);

    DamageNumbers::spawn(position(), amount);
}

bool Entity::overlaps(const Entity& other) const
{
    return hitboxes_intersect(
        _hurt_box,  position(),
        other._hurt_box, other.position()
    );
}

bool Entity::attack_hits(const Entity& other) const
{
    return hitboxes_intersect(
        _attack_box, position(),
        other._hurt_box, other.position()
    );
}

void Entity::_tick_invulnerability()
{
    if(_invuln_timer > 0)
    {
        --_invuln_timer;
    }
}

void Entity::_start_knockback(const bn::fixed_point& source_pos)
{
    if(!_sprite)
    {
        return;
    }

    bn::fixed_point pos = _sprite->position();
    bn::fixed dx = pos.x() - source_pos.x();
    bn::fixed dy = pos.y() - source_pos.y();

    bn::fixed len_sq = dx * dx + dy * dy;
    if(len_sq == 0)
    {
        _knockback_dir = bn::fixed_point(0, -1);
    }
    else
    {
        bn::fixed len = bn::sqrt(len_sq);
        _knockback_dir = bn::fixed_point(dx / len, dy / len);
    }

    _knockback_timer = _knockback_duration;
}

void Entity::_apply_knockback()
{
    if(_knockback_timer <= 0 || !_sprite)
    {
        return;
    }

    --_knockback_timer;

    const bn::fixed_point delta(
        _knockback_dir.x() * _knockback_strength,
        _knockback_dir.y() * _knockback_strength
    );

    // Knockback now respects collisions and world bounds
    move_by(delta);
}
