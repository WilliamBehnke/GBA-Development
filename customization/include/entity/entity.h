#ifndef ENTITY_H
#define ENTITY_H

#include "hitbox.h"
#include "damage_numbers.h"
#include "health_bar.h"
#include "entity_sprite.h"

#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_math.h"

class WorldMap;

class Entity 
{
public:
    Entity(EntitySprite* sprite,
           const WorldMap* world_map,
           int max_health,
           int damage,
           const Hitbox& hurt_box,
           const Hitbox& attack_box,
           int invuln_duration_frames = 30);

    virtual ~Entity() = default;

    virtual void attach_camera(const bn::camera_ptr& camera);

    // Per-frame update (call this from sub-classes)
    void update_entity();

    int health() const     { return _health; }
    int max_health() const { return _max_health; }
    bool is_alive() const  { return _health > 0; }

    bool is_invulnerable() const { return _invuln_timer > 0; }

    int damage() const          { return _damage; }
    void set_damage(int damage) { _damage = damage; }

    void take_damage(int amount);
    void take_damage(int amount, const bn::fixed_point& source_pos);

    const Hitbox& hurt_box() const   { return _hurt_box; }
    const Hitbox& attack_box() const { return _attack_box; }

    bn::fixed_point position() const;
    void move_by(const bn::fixed_point& delta);

    bool is_attacking() const;

    // Any overlap between our hurt box and the other's hurt box
    bool overlaps(const Entity& other) const;

    // Does our attack box hit their hurt box?
    bool attack_hits(const Entity& other) const;

    void set_active(bool active);
    bool is_active() const { return _active; }

protected:
    EntitySprite* _sprite;
    const WorldMap* _world_map;

    int _health     = 0;
    int _max_health = 0;
    int _damage     = 1;

    bool _show_health_bar = true;
    HealthBar _health_bar;

    bool _active = true;

    Hitbox _hurt_box;
    Hitbox _attack_box;

    bn::fixed_point _clamp_to_world(const bn::fixed_point& candidate) const;
    virtual bn::fixed_point _get_feet_position(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const = 0;
    bool _can_stand_at(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const;

private:
    void _tick_invulnerability();

    int _invuln_timer     = 0;
    int _invuln_duration  = 0;

    void _start_knockback(const bn::fixed_point& source_pos);
    void _apply_knockback();

    bn::fixed_point _knockback_dir      = bn::fixed_point(0, 0);  // normalized
    int             _knockback_timer    = 0;
    int             _knockback_duration = 6;        // frames of knockback
    bn::fixed       _knockback_strength = 2;        // pixels per frame
};

#endif // ENTITY_H
