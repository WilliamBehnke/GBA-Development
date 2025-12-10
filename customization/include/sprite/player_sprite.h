#ifndef PLAYER_SPRITE_H
#define PLAYER_SPRITE_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "entity_sprite.h"
#include "character_appearance.h"
#include "character_colors.h"
#include "character_assets.h"

// ---------------------------------------------------------------------------
// PlayerSprite
// Handles layered sprites, palettes and animation for a player character.
// ---------------------------------------------------------------------------

class PlayerSprite : public EntitySprite
{
public:
    // appearance is a snapshot from customization; it is not modified here
    explicit PlayerSprite(const CharacterAppearance& appearance);

    // Create sprites based on current appearance and initial position
    void rebuild(const bn::fixed_point& pos);

    // Uniform scale for all character layers
    void set_scale(int scale);

    // Attach/detach camera to all sprites
    void attach_camera(const bn::camera_ptr& camera) override;
    void attach_camera();
    void detach_camera() override;

    bn::fixed_point position() override;
    void set_position(bn::fixed_point pos) override;
    void set_z_order(int z) override;

private:
    const CharacterAppearance& _appearance;
    bn::sprite_palette_ptr _palette;

    // Layered sprites
    bn::optional<bn::sprite_ptr> _body_sprite;
    bn::optional<bn::sprite_ptr> _eyes_sprite;
    bn::optional<bn::sprite_ptr> _top_sprite;
    bn::optional<bn::sprite_ptr> _bottom_sprite;
    bn::optional<bn::sprite_ptr> _hair_sprite;

    // Sprite items used (so we can set tiles each frame)
    const bn::sprite_item* _body_item   = nullptr;
    const bn::sprite_item* _eyes_item   = nullptr;
    const bn::sprite_item* _top_item    = nullptr;
    const bn::sprite_item* _bottom_item = nullptr;
    const bn::sprite_item* _hair_item   = nullptr;

    void _rebuild_sprites(const bn::fixed_point& pos);

    void _update_movement_animation(bool moving) override;
    void _update_attack_animation() override;
    void _update_hurt_animation() override;
    void _update_death_animation() override;
    void _update_block_animation() override;
    void _update_block_success_animation() override;

    void _sync_sprite(const bn::fixed_point& pos) override;
};

#endif // PLAYER_SPRITE_H
