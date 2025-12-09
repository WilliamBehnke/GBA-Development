#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_camera_ptr.h"

#include "hitbox.h"
#include "entity.h"
#include "character_customization/character_appearance.h"
#include "sprite/player_sprite.h"

class WorldMap;

class Player : public Entity
{
public:
    Player(PlayerSprite* sprite, const bn::fixed_point& start_pos, const WorldMap* world);

    // Update with collisions + camera against the saved world map
    void update();

    // Attach a camera that follows the player (and is clamped to map edges)
    void attach_camera(const bn::camera_ptr& camera);

    const bn::fixed_point& position() const
    {
        return _pos;
    }

    void set_position(const bn::fixed_point& pos)
    {
        _pos = pos;
    }

    void set_direction(FacingDirection direction)
    {
        _direction = direction;
    }

    void update_sprite();

    const Hitbox& hitbox() const { return _hitbox; }

private:
    // World state
    bn::fixed_point _pos;
    FacingDirection _direction;

    const WorldMap* _world_map;

    Hitbox _hitbox;

    // Intended movement this frame
    bn::fixed _move_dx = 0;
    bn::fixed _move_dy = 0;
    bool _moving = false;

    // Camera
    bn::optional<bn::camera_ptr> _camera;

    // Sprite/animation handler
    PlayerSprite* _sprite;

    static constexpr bn::fixed k_speed = bn::fixed(1);

    void _handle_input();     // read keypad, set _move_dx/_move_dy
    void _apply_movement();   // apply movement with collisions
    void _update_camera();    // clamp camera to map edges
};

#endif // PLAYER_H
