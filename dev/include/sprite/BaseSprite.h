#ifndef BASE_SPRITE_H
#define BASE_SPRITE_H

#include "bn_array.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_item.h"
#include "bn_sprite_animate_actions.h"

enum class direction : int {
    DOWN  = 0,
    RIGHT = 1,
    LEFT  = 2,
    UP    = 3,
};

enum class anim_kind : int {
    Idle,
    Walk,
    Hurt,
    Attack,
    Death,
};

class BaseSprite {
public:
    static constexpr int MaxFrames         = 10;
    static constexpr int MaxAttackVariants = 3;

    struct Clip {
        bn::array<uint16_t, MaxFrames> frames{};
        int frame_count   = 0;
        int wait_updates  = 6;
        bool loop         = true;
        bool freeze_on_last_frame = false;
    };

    struct DirectionSet {
        Clip idle;
        Clip walk;
        Clip hurt;
        Clip death;
        bn::array<Clip, MaxAttackVariants> attacks;
    };

    BaseSprite(const bn::sprite_tiles_item& tiles_item,
               bn::sprite_ptr sprite,
               const bn::array<DirectionSet, 4>& anim_data,
               int attack_variants);

    virtual ~BaseSprite() = default;

    void set_direction(direction d);

    void play_idle();
    void play_walk(direction d);
    void play_hurt();
    void play_attack();
    void play_death();

    void update();

    bool is_locked() const;
    bool is_attacking() const;
    bool is_hurting() const;
    bool is_dead() const;

    bn::sprite_ptr& sprite();
    const bn::sprite_ptr& sprite() const;

protected:
    const bn::array<DirectionSet, 4>& _data;

    const DirectionSet& current() const {
        return _data[int(_dir)];
    }

private:
    void _start_clip(const Clip& clip, bool force_loop);

    const bn::sprite_tiles_item& _tiles_item;
    bn::sprite_ptr _sprite;

    direction _dir  = direction::DOWN;
    anim_kind _kind = anim_kind::Idle;

    int _attack_variants = 1;   // how many attack clips are valid (1..MaxAttackVariants)
    int _attack_index    = 0;   // which attack variant is next

    void _end_death();

    bn::optional<bn::sprite_animate_action<MaxFrames>> _action;
};

#endif // BASE_SPRITE_H
