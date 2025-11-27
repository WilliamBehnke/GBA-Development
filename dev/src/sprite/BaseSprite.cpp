#include "BaseSprite.h"

#include "bn_span.h"
#include "bn_log.h"
#include "bn_math.h"

BaseSprite::BaseSprite(const bn::sprite_tiles_item& tiles_item,
                       bn::sprite_ptr sprite,
                       const bn::array<DirectionSet, 4>& anim_data,
                       int attack_variants) :
    _tiles_item(tiles_item),
    _sprite(bn::move(sprite)),
    _data(anim_data),
    _attack_variants(attack_variants > 0 ? attack_variants : 1) {
    if(_attack_variants > MaxAttackVariants) {
        _attack_variants = MaxAttackVariants;
    }
}

void BaseSprite::set_direction(direction d) {
    _dir = d;
}

void BaseSprite::play_idle() {
    if(_kind == anim_kind::Idle || _kind == anim_kind::Death) {
        return;
    }

    _kind = anim_kind::Idle;
    _start_clip(current().idle, true);
}

void BaseSprite::play_walk(direction d) {
    if(_kind == anim_kind::Death) {
        return;
    }

    if(_kind == anim_kind::Walk && _dir == d) {
        return;
    }

    _dir  = d;
    _kind = anim_kind::Walk;
    _start_clip(current().walk, true);
}

void BaseSprite::play_hurt() {
    if(_kind == anim_kind::Death) {
        return;
    }

    _kind = anim_kind::Hurt;
    _start_clip(current().hurt, false);
}

void BaseSprite::play_attack() {
    if(_attack_variants <= 0 || _kind == anim_kind::Death) {
        return;
    }

    if(_attack_index >= _attack_variants) {
        _attack_index = 0;
    }

    _kind = anim_kind::Attack;
    _start_clip(current().attacks[_attack_index], false);

    _attack_index = (_attack_index + 1) % _attack_variants;
}

void BaseSprite::play_death() {
    if(_kind == anim_kind::Death) {
        return;
    }

    _kind = anim_kind::Death;

    const Clip& clip = current().death;

    if(clip.frame_count <= 0) {
        _sprite.set_visible(false);
        _action.reset();
        return;
    }

    _start_clip(clip, false);
}

void BaseSprite::update() {
    if(!_action) {
        return;
    }

    _action->update();

    if(!_action->update_forever() && _action->done()) {
        if(_kind == anim_kind::Attack || _kind == anim_kind::Hurt) {
            play_idle();
        }

        if(_kind == anim_kind::Death) {
            _end_death();
        }
    }
}

bool BaseSprite::is_locked() const {
    return _kind == anim_kind::Attack ||
           _kind == anim_kind::Hurt   ||
           _kind == anim_kind::Death;
}

bool BaseSprite::is_attacking() const {
    return _kind == anim_kind::Attack;
}

bool BaseSprite::is_hurting() const {
    return _kind == anim_kind::Hurt;
}

bool BaseSprite::is_dead() const {
    return _kind == anim_kind::Death;
}

bn::sprite_ptr& BaseSprite::sprite() {
    return _sprite;
}

const bn::sprite_ptr& BaseSprite::sprite() const {
    return _sprite;
}

void BaseSprite::_end_death() {
    const Clip& clip = current().death;

    if(clip.frame_count > 0) {
        if(clip.freeze_on_last_frame) {
            // Freeze on final frame
            const uint16_t last_frame = clip.frames[clip.frame_count - 1];
            _sprite.set_tiles(_tiles_item, last_frame);
        } else {
            // Hide sprite when animation ends
            _sprite.set_visible(false);
        }
    } else {
        // No frames defined, so hide sprite
        _sprite.set_visible(false);
    }

    _action.reset();
}

void BaseSprite::_start_clip(const Clip& clip, bool force_loop) {
    bn::span<const uint16_t> frames(clip.frames.data(), clip.frame_count);
    const bool loop = force_loop ? true : clip.loop;

    if(loop) {
        _action = bn::sprite_animate_action<MaxFrames>::forever(
            _sprite, clip.wait_updates, _tiles_item, frames
        );
    } else {
        _action = bn::sprite_animate_action<MaxFrames>::once(
            _sprite, clip.wait_updates, _tiles_item, frames
        );
    }
}
