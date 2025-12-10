#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_camera_ptr.h"

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

    const bn::fixed_point position() const
    {
        return _sprite->position();
    }

    void update_sprite(const bn::fixed_point& pos, FacingDirection direction)
    {
        _direction = direction;
        _sprite->update(pos, direction, _moving);
    }

    bool is_blocking_attack_from(const bn::fixed_point& source_pos) const override;
    void on_block_broken() override;

private:
    FacingDirection _direction;

    // Intended movement this frame
    bn::fixed _move_dx = 0;
    bn::fixed _move_dy = 0;
    bool _moving = false;
    bool _is_blocking = false;

    // Camera
    bn::optional<bn::camera_ptr> _camera;

    // Sprite/animation handler
    PlayerSprite* _sprite;

    static constexpr bn::fixed k_speed = bn::fixed(0.6);

    bn::fixed_point _get_feet_position(const bn::fixed_point& old_pos, const bn::fixed_point& new_pos) const override;

    void _handle_input();     // read keypad, set _move_dx/_move_dy
    void _apply_movement(bn::fixed_point& new_pos);
    void _update_camera();
};

#endif // PLAYER_H
