#ifndef ENTITY_H
#define ENTITY_H

#include "entity/Hitbox.h"
#include "sprite/BaseSprite.h"
#include "ui/DamageNumbers.h"

#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_math.h"

class Entity {
public:
    Entity(BaseSprite* sprite,
           int max_health,
           int damage,
           const Hitbox& hurt_box,
           const Hitbox& attack_box,
           int invuln_duration_frames = 30);

    virtual ~Entity() = default;

    // Per-frame update (call this from sub-classes)
    virtual void update();

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

protected:
    BaseSprite* _sprite = nullptr;

    int _health     = 0;
    int _max_health = 0;
    int _damage     = 1;

    Hitbox _hurt_box;
    Hitbox _attack_box;

private:
    void _tick_invulnerability();

    int _invuln_timer     = 0;
    int _invuln_duration  = 0;

    void _start_knockback(const bn::fixed_point& source_pos);
    void _apply_knockback();

    bn::fixed_point _knockback_dir      = bn::fixed_point(0, 0);  // normalized
    int             _knockback_timer    = 0;
    int             _knockback_duration = 8;        // frames of knockback
    bn::fixed       _knockback_strength = 1;        // pixels per frame
};

#endif // ENTITY_H
