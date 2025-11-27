#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "BaseSprite.h"
#include "bn_fixed_point.h"
#include "bn_vector.h"

class Enemy : public Entity
{
public:
    Enemy(BaseSprite* sprite);

    // Who this enemy should chase / attack
    void set_target(Entity* target) { _target = target; }

    // Patrol path setup
    static constexpr int max_patrol_points = 8;

    void add_patrol_point(const bn::fixed_point& point);
    void clear_patrol_points();

    void update();

private:
    void _update_ai();
    void _patrol();
    void _chase(const bn::fixed_point& to_target);
    void _apply_movement_animation();

    Entity* _target = nullptr;

    bn::fixed_point _velocity;

    bn::vector<bn::fixed_point, max_patrol_points> _patrol_points;
    int _current_patrol_index = 0;
    bool _loop_patrol = true;

    bn::fixed _max_speed;      // how fast the enemy can move
    bn::fixed _accel;          // smoothing factor

    bn::fixed _aggro_radius;   // start chasing when inside this
    bn::fixed _lose_radius;    // stop chasing when outside this
    bn::fixed _attack_range;   // try to attack when inside this

    int _attack_cooldown = 0;
    int _attack_cooldown_max;
};

#endif // ENEMY_H
