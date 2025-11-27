#ifndef BASE_SPRITE_H
#define BASE_SPRITE_H

#include "bn_array.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_item.h"
#include "bn_sprite_animate_actions.h"
#include "bn_span.h"
#include "bn_log.h"

namespace
{
    enum class direction : int
    {
        DOWN = 0,
        RIGHT = 1,
        LEFT = 2,
        UP = 3,
    };

    enum class anim_kind : int
    {
        Idle,
        Walk,
        Hurt,
        Attack,
        Death,   // <-- NEW
    };
}

class BaseSprite
{
public:
    static constexpr int MaxFrames         = 10;
    static constexpr int MaxAttackVariants = 3;

    struct Clip
    {
        bn::array<uint16_t, MaxFrames> frames{};
        int frame_count   = 0;
        int wait_updates  = 6;
        bool loop         = true;
        bool freeze_on_last_frame = false;
    };

    struct DirectionSet
    {
        Clip idle;
        Clip walk;
        Clip hurt;
        Clip death;
        bn::array<Clip, MaxAttackVariants> attacks;
    };

    BaseSprite(const bn::sprite_tiles_item& tiles_item,
               bn::sprite_ptr sprite,
               const bn::array<DirectionSet, 4>& anim_data,
               int attack_variants) :
        _tiles_item(tiles_item),
        _sprite(bn::move(sprite)),
        _data(anim_data),
        _attack_variants(attack_variants > 0 ? attack_variants : 1)
    {
        if(_attack_variants > MaxAttackVariants)
        {
            _attack_variants = MaxAttackVariants;
        }
    }

    virtual ~BaseSprite() = default;

    void set_direction(direction d)
    {
        _dir = d;
    }

    void play_idle()
    {
        // don't override death once dead
        if(_kind == anim_kind::Idle || _kind == anim_kind::Death)
        {
            return;
        }

        _kind = anim_kind::Idle;
        _start_clip(current().idle, true);
    }

    void play_walk(direction d)
    {
        // don't walk if dead
        if(_kind == anim_kind::Death)
        {
            return;
        }

        if(_kind == anim_kind::Walk && _dir == d)
        {
            return;
        }

        _dir = d;
        _kind = anim_kind::Walk;
        _start_clip(current().walk, true);
    }

    void play_hurt()
    {
        // don't play hurt over death
        if(_kind == anim_kind::Death)
        {
            return;
        }

        _kind = anim_kind::Hurt;
        _start_clip(current().hurt, false);
    }

    void play_attack()
    {
        if(_attack_variants <= 0 || _kind == anim_kind::Death)
        {
            return;
        }

        if(_attack_index >= _attack_variants)
        {
            _attack_index = 0;
        }

        _kind = anim_kind::Attack;
        _start_clip(current().attacks[_attack_index], false);

        _attack_index = (_attack_index + 1) % _attack_variants;
    }

    void play_death()
    {
        BN_LOG("Playing death animation");

        if(_kind == anim_kind::Death)
        {
            return;
        }

        _kind = anim_kind::Death;

        const Clip& clip = current().death;

        if(clip.frame_count <= 0)
        {
            BN_LOG("No death animation — hiding sprite");
            _sprite.set_visible(false);
            _action.reset();
            return;
        }

        _start_clip(clip, false);
    }

    void update()
    {
        if(_action)
        {
            _action->update();

            if(!_action->update_forever() && _action->done())
            {
                // After Attack or Hurt, go back to idle.
                // After Death, stay on last frame.
                if(_kind == anim_kind::Attack || _kind == anim_kind::Hurt)
                {
                    play_idle();
                }

                if(_kind == anim_kind::Death)
                {
                    const Clip& clip = current().death;

                    if(clip.frame_count > 0)
                    {
                        if(clip.freeze_on_last_frame)
                        {
                            // Freeze on final frame
                            uint16_t last_frame = clip.frames[clip.frame_count - 1];
                            _sprite.set_tiles(_tiles_item, last_frame);
                        }
                        else
                        {
                            // Hide sprite when animation ends
                            _sprite.set_visible(false);
                        }
                    }
                    else
                    {
                        // No frames defined → hide sprite
                        _sprite.set_visible(false);
                    }

                    // Stop animation
                    _action.reset();
                }
            }
        }
    }

    bool is_locked() const
    {
        // Death is also locked so nothing overrides it
        return _kind == anim_kind::Attack ||
               _kind == anim_kind::Hurt   ||
               _kind == anim_kind::Death;
    }

    bool is_attacking() const
    {
        return _kind == anim_kind::Attack;
    }

    bool is_hurting() const
    {
        return _kind == anim_kind::Hurt;
    }

    // NEW helper
    bool is_dead() const
    {
        return _kind == anim_kind::Death;
    }

    bn::sprite_ptr& sprite()
    {
        return _sprite;
    }

    const bn::sprite_ptr& sprite() const
    {
        return _sprite;
    }

protected:
    const bn::array<DirectionSet, 4>& _data;

    const DirectionSet& current() const
    {
        return _data[int(_dir)];
    }

private:
    void _start_clip(const Clip& clip, bool force_loop)
    {
        bn::span<const uint16_t> frames(clip.frames.data(), clip.frame_count);
        const bool loop = force_loop ? true : clip.loop;

        if(loop)
        {
            _action = bn::sprite_animate_action<MaxFrames>::forever(
                _sprite, clip.wait_updates, _tiles_item, frames
            );
        }
        else
        {
            _action = bn::sprite_animate_action<MaxFrames>::once(
                _sprite, clip.wait_updates, _tiles_item, frames
            );
        }
    }

    const bn::sprite_tiles_item& _tiles_item;
    bn::sprite_ptr _sprite;

    direction _dir = direction::DOWN;
    anim_kind _kind = anim_kind::Idle;

    int _attack_variants = 1;   // how many attack clips are valid (1..MaxAttackVariants)
    int _attack_index    = 0;   // which attack variant is next

    bn::optional<bn::sprite_animate_action<MaxFrames>> _action;
};

#endif // BASE_SPRITE_H
