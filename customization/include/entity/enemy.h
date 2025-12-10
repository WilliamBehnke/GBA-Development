#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "character_appearance.h"
#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_optional.h"

class WorldMap;
class EntitySprite;

class Enemy : public Entity
{
public:
    Enemy(EntitySprite* sprite, const WorldMap* world_map);

    void attach_camera(const bn::camera_ptr& camera);
    void update();

    void set_target(Entity* target) { _target = target; }

private:
    bn::fixed_point _get_feet_position(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const override;

    void _update_ai();
    void _chase(const bn::fixed_point& to_target);          // kept as fallback
    void _apply_movement_animation();                       // now also does collisions

    void _update_path_and_velocity(const bn::fixed_point& my_pos,
                                   const bn::fixed_point& target_pos);

    Entity* _target = nullptr;
    FacingDirection _direction;

    bn::fixed_point _velocity;
    bn::fixed        _max_speed;
    bn::fixed        _aggro_radius;
    bn::fixed        _lose_radius;

    int _attack_cooldown;
    int _attack_cooldown_max;

    bool _start_attack = false;

    // pathfinding: recalc direction only every few frames
    int _path_recalc_timer       = 0;
    static constexpr int k_path_recalc_interval = 10;
};

#endif // ENEMY_H
