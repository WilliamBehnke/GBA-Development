#ifndef ENEMY_SPRITE_H
#define ENEMY_SPRITE_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_item.h"

#include "character_appearance.h"
#include "entity_sprite.h"

// ---------------------------------------------------------------------------
// EnemySprite
// Handles animation for an enemy.
// ---------------------------------------------------------------------------

class EnemySprite : public EntitySprite
{
public:
    EnemySprite(const bn::fixed_point& pos);

    // Attach/detach camera
    void attach_camera(const bn::camera_ptr& camera) override;
    void detach_camera() override;

    bn::fixed_point position() override;
    void set_position(bn::fixed_point pos) override;
    void set_z_order(int z) override;

    void set_visible(bool is_visible) override;

private:
    bn::optional<bn::sprite_ptr> _sprite;

    const bn::sprite_item _sprite_item;

    // Different animation updaters
    void _update_movement_animation(bool moving) override;
    void _update_attack_animation() override;
    void _update_hurt_animation() override;
    void _update_death_animation() override;

    void _sync_sprite(const bn::fixed_point& pos) override;
};

#endif // ENEMY_SPRITE_H
