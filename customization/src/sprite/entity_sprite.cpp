// ---------------------------------------------------------------------------
// entity_sprite.cpp
// ---------------------------------------------------------------------------

#include "entity_sprite.h"

#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_base_0.h"

EntitySprite::~EntitySprite() = default;

void EntitySprite::update(const bn::fixed_point& pos,
                          FacingDirection direction,
                          bool moving)
{
    _direction = direction;

    switch(_state)
    {
        case AnimationState::Attack:
            _update_attack_animation();
            break;

        case AnimationState::Hurt:
            _update_hurt_animation();
            break;

        case AnimationState::Death:
            _update_death_animation();
            break;

        case AnimationState::Block:
            _update_block_animation();
            break;

        case AnimationState::BlockSuccess:
            // Just hold the success frame; you could add a timer if desired.
            break;

        case AnimationState::Walk:
        case AnimationState::Idle:
        default:
            _update_movement_animation(moving);
            break;
    }

    _sync_sprite(pos);
}

void EntitySprite::play_attack()
{
    if(_state == AnimationState::Death)
        return;

    _state = AnimationState::Attack;
    _anim_counter = 0;
    _attack_frame = 0;
}

void EntitySprite::play_hurt()
{
    if(_state == AnimationState::Death)
        return;

    _state = AnimationState::Hurt;
    _anim_counter = 0;
    _hurt_frame = 0;
}

void EntitySprite::play_death()
{
    _state = AnimationState::Death;
    _anim_counter = 0;
    _death_frame = 0;
}

void EntitySprite::play_block()
{
    if(_state == AnimationState::Death)
        return;
    
    _state = AnimationState::Block;
    _anim_counter = 0;
    _block_frame = 0;
}

void EntitySprite::play_block_success()
{
    if(_state == AnimationState::Death)
        return;

    _state = AnimationState::BlockSuccess;
    _anim_counter = 0;
}

void EntitySprite::play_idle()
{
    if(_state == AnimationState::Death)
        return;

    _state = AnimationState::Idle;
    _anim_counter = 0;
    _idle_frame = 0;
}


bool EntitySprite::is_locked() const
{
    return (
        _state == AnimationState::Attack ||
        _state == AnimationState::Hurt   ||
        _state == AnimationState::Death
    );
}
