#ifndef PLAYER_SPRITE_H
#define PLAYER_SPRITE_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "character_customization/character_appearance.h"
#include "character_customization/character_colors.h"
#include "character_customization/character_assets.h"

// ---------------------------------------------------------------------------
// PlayerSprite
// Handles layered sprites, palettes and animation for a player character.
// ---------------------------------------------------------------------------

class PlayerSprite
{
public:
    // appearance is a snapshot from customization; it is not modified here
    explicit PlayerSprite(const CharacterAppearance& appearance);

    // Create sprites based on current appearance and initial position
    void rebuild(const bn::fixed_point& pos);

    // Uniform scale for all character layers
    void set_scale(int scale);

    // Attach/detach camera to all sprites
    void attach_camera(const bn::camera_ptr& camera);
    void attach_camera();
    void detach_camera();

    // Update animation + sync positions / frames to the given state
    void update(const bn::fixed_point& pos, FacingDirection direction, bool moving);

private:
    const CharacterAppearance& _appearance;

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

    // Animation
    int _anim_counter = 0;
    int _idle_frame   = 0;   // 0..1
    int _walk_frame   = 0;   // 0..3
    bool _moving      = false;
    FacingDirection _direction = FacingDirection::Down;

    // Optional camera (mirrors Player's camera)
    bn::optional<bn::camera_ptr> _camera;

    void _rebuild_sprites(const bn::fixed_point& pos); // pick items, create sprites
    void _apply_colors();                              // recolor palettes
    void _update_animation(bool moving);               // idle(2)/walk(4) anim step
    void _sync_sprites(const bn::fixed_point& pos);    // position + frame index + camera
};

#endif // PLAYER_SPRITE_H
