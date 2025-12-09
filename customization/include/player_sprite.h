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
    // High-level animation state
    enum class AnimationState
    {
        Idle,
        Walk,
        Attack,
        Hurt,
        Death
    };

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

    // Update animation + sync positions / frames to the given state.
    // `moving` only affects Idle/Walk; Attack/Hurt/Death override it.
    void update(const bn::fixed_point& pos, FacingDirection direction, bool moving);

    // Trigger special animations
    void play_attack();
    void play_hurt();
    void play_death();

    // Optional helpers
    AnimationState animation_state() const { return _state; }

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

    // Animation
    AnimationState _state = AnimationState::Idle;

    int _anim_counter = 0;

    int _idle_frame   = 0;   // 0..3    (relative in idle segment)
    int _walk_frame   = 0;   // 0..5    (relative in walk segment)
    int _attack_frame = 0;   // 0..9    (relative in attack segment)
    int _hurt_frame   = 0;   // 0..3    (relative in hurt segment)
    int _death_frame  = 0;   // 0..3    (relative in death segment)

    bool _moving      = false;
    FacingDirection _direction = FacingDirection::Down;

    // Optional camera (mirrors Player's camera)
    bn::optional<bn::camera_ptr> _camera;

    void _rebuild_sprites(const bn::fixed_point& pos); // pick items, create sprites

    // Different animation updaters
    void _update_movement_animation(bool moving);      // idle / walk
    void _update_attack_animation();                   // attack
    void _update_hurt_animation();                     // hurt
    void _update_death_animation();                    // death

    void _sync_sprites(const bn::fixed_point& pos);    // position + frame index + camera
};

#endif // PLAYER_SPRITE_H
