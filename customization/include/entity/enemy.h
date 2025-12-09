#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "enemy_sprite.h"

#include "bn_fixed_point.h"

class Enemy : public Entity
{
public:
    Enemy(EntitySprite* sprite);

    void attach_camera(const bn::camera_ptr& camera);

    // Who this enemy should chase / attack
    void set_target(Entity* target) { _target = target; }

    void update();

private:
    void _update_ai();
    void _chase(const bn::fixed_point& to_target);
    void _apply_movement_animation();

    FacingDirection _direction;

    Entity* _target = nullptr;

    bn::fixed_point _velocity;

    bn::fixed _max_speed;      // how fast the enemy can move
    bn::fixed _aggro_radius;   // start chasing when inside this
    bn::fixed _lose_radius;    // stop chasing when outside this

    int _attack_cooldown      = 0;
    int _attack_cooldown_max  = 0;
    bool _start_attack = false;
};

#endif // ENEMY_H
