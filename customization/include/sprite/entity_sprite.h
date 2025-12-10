#ifndef ENITY_SPRITE_H
#define ENITY_SPRITE_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "character_appearance.h"

class EntitySprite
{
public:
    // High-level animation state
    enum class AnimationState
    {
        Idle,
        Walk,
        Attack,
        Hurt,
        Death,
        Block,
        BlockSuccess,
    };

    EntitySprite() = default;
    virtual ~EntitySprite() = 0;

    // Attach/detach camera
    virtual void attach_camera(const bn::camera_ptr& camera) = 0;
    virtual void detach_camera() = 0;

    void update(const bn::fixed_point& pos, FacingDirection direction, bool moving);

    virtual bn::fixed_point position() = 0;
    virtual void set_position(bn::fixed_point pos) = 0;
    virtual void set_z_order(int z) = 0;

    // Trigger special animations
    void play_attack();
    void play_hurt();
    void play_death();
    void play_block();
    void play_block_success();
    void play_idle();

    AnimationState animation_state() const { return _state; }

    bool is_locked() const;

protected:
    // Animation
    AnimationState _state = AnimationState::Idle;

    int _anim_counter = 0;

    int _idle_frame   = 0;   // 0..3    (relative in idle segment)
    int _walk_frame   = 0;   // 0..5    (relative in walk segment)
    int _attack_frame = 0;   // 0..9    (relative in attack segment)
    int _hurt_frame   = 0;   // 0..3    (relative in hurt segment)
    int _death_frame  = 0;   // 0..3    (relative in death segment)
    int _block_frame  = 0;   // 0..2    (relative in block segment)

    int _block_success_timer = 0;

    bool _moving      = false;
    FacingDirection _direction = FacingDirection::Down;

    bn::optional<bn::camera_ptr> _camera;

    virtual void _update_movement_animation(bool moving) = 0;
    virtual void _update_attack_animation() = 0;
    virtual void _update_hurt_animation() = 0;
    virtual void _update_death_animation() = 0;
    virtual void _update_block_animation() = 0;
    virtual void _update_block_success_animation() = 0;

    virtual void _sync_sprite(const bn::fixed_point& pos) = 0;
};

#endif // ENITY_SPRITE_H
