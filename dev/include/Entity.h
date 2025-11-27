#ifndef ENTITY_H
#define ENTITY_H

#include "Hitbox.h"
#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_math.h"
#include "BaseSprite.h"
#include "DamageNumbers.h"

class Entity
{
public:
    // invuln_duration_frames = how many frames of invulnerability after a hit
    Entity(BaseSprite* sprite,
           int max_health,
           const Hitbox& hurt_box,
           const Hitbox& attack_box,
           int invuln_duration_frames = 30) :
        _sprite(sprite),
        _health(max_health),
        _max_health(max_health),
        _hurt_box(hurt_box),
        _attack_box(attack_box),
        _invuln_duration(invuln_duration_frames)
    {}

    virtual ~Entity() = default;

    // ------------------------------------------------------
    // Per-frame update (call this from sub-classes)
    // ------------------------------------------------------
    virtual void update()
    {
        _apply_knockback();

        if(_sprite)
        {
            _sprite->update();

            // Y-sort: bigger Y = closer to camera (on top)
            bn::fixed_point pos = _sprite->sprite().position();
            _sprite->sprite().set_z_order(-pos.y().integer());
        }

        _tick_invulnerability();
    }

    // ------------------------------------------------------
    // Health
    // ------------------------------------------------------
    int health() const     { return _health; }
    int max_health() const { return _max_health; }
    bool is_alive() const  { return _health > 0; }

    bool is_invulnerable() const
    {
        return _invuln_timer > 0;
    }

    // ------------------------------------------------------
    // Damage logic + knockback + damage numbers
    // ------------------------------------------------------
    void take_damage(int amount)
    {
        // Fallback: no direction info (no meaningful knockback dir)
        take_damage(amount, position());
    }

    // Directional damage: source_pos is where the hit came FROM
    void take_damage(int amount, const bn::fixed_point& source_pos)
    {
        if(amount <= 0 || !is_alive() || is_invulnerable())
            return;

        _health -= amount;
        if(_health < 0)
            _health = 0;

        _invuln_timer = _invuln_duration;

        if(_sprite)
        {
            if(_health <= 0)
            {
                // Play death instead of hurt when killed
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

    // ------------------------------------------------------
    // Hitboxes / collision helpers
    // ------------------------------------------------------
    const Hitbox& hurt_box() const   { return _hurt_box; }
    const Hitbox& attack_box() const { return _attack_box; }

    bn::fixed_point position() const
    {
        return _sprite ? _sprite->sprite().position() : bn::fixed_point();
    }

    // Move the entity by a small delta (used by collision / bump resolution)
    void move_by(const bn::fixed_point& delta)
    {
        if(_sprite)
        {
            bn::fixed_point pos = _sprite->sprite().position();
            _sprite->sprite().set_position(pos + delta);
        }
    }

    bool is_attacking() const
    {
        return _sprite ? _sprite->is_attacking() : false;
    }

    // Any overlap between our hurt box and the other's hurt box
    bool overlaps(const Entity& other) const
    {
        return hitboxes_intersect(
            _hurt_box,  position(),
            other._hurt_box, other.position()
        );
    }

    // Does our attack box hit their hurt box?
    bool attack_hits(const Entity& other) const
    {
        return hitboxes_intersect(
            _attack_box, position(),
            other._hurt_box, other.position()
        );
    }

protected:
    BaseSprite* _sprite = nullptr;

    int _health;
    int _max_health;

    Hitbox _hurt_box;
    Hitbox _attack_box;

private:
    // ------------------------------------------------------
    // Invulnerability decay
    // ------------------------------------------------------
    void _tick_invulnerability()
    {
        if(_invuln_timer > 0)
            --_invuln_timer;
    }

    int _invuln_timer = 0;
    int _invuln_duration = 0;

    // ------------------------------------------------------
    // Knockback (diagonal)
    // ------------------------------------------------------
    void _start_knockback(const bn::fixed_point& source_pos)
    {
        if(!_sprite)
            return;

        bn::fixed_point pos = _sprite->sprite().position();
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

    void _apply_knockback()
    {
        if(_knockback_timer <= 0 || !_sprite)
            return;

        --_knockback_timer;

        bn::fixed_point pos = _sprite->sprite().position();
        pos.set_x(pos.x() + _knockback_dir.x() * _knockback_strength);
        pos.set_y(pos.y() + _knockback_dir.y() * _knockback_strength);
        _sprite->sprite().set_position(pos);
    }

    bn::fixed_point _knockback_dir = bn::fixed_point(0, 0);  // normalized
    int _knockback_timer = 0;
    int _knockback_duration = 8;        // frames of knockback
    bn::fixed _knockback_strength = 1;  // pixels per frame
};

#endif // ENTITY_H
