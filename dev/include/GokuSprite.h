#ifndef GOKU_SPRITE_H
#define GOKU_SPRITE_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_optional.h"
#include "bn_sprite_items_goku.h"

#include "CharacterSprite.h"

// Concrete implementation of CharacterSprite using the goku sprite sheet.
class GokuSprite : public CharacterSprite
{
public:
    GokuSprite();

    // Position ---------------------------------------------------------
    [[nodiscard]] bn::fixed_point position() const override;
    void set_position(const bn::fixed_point& pos) override;

    // Animation state --------------------------------------------------
    void set_idle(direction dir) override;
    void set_walk(direction dir) override;
    void start_hurt(direction dir) override;
    void start_attack(direction dir) override;

    [[nodiscard]] bool is_attacking() const override;
    [[nodiscard]] bool is_hurt() const override;
    void update() override;

private:
    enum class State
    {
        IDLE,
        WALK,
        ATTACK,
        HURT
    };

    enum class AttackCombo
    {
        RIGHT_HOOK,
        LEFT_HOOK,
        KICK
    };

    static AttackCombo _next_combo(AttackCombo c);

    // Animation helpers
    bn::sprite_animate_action<4> _make_attack_anim(direction facing, AttackCombo combo);
    bn::sprite_animate_action<4> _make_walk_anim(direction facing);
    bn::sprite_animate_action<10> _make_idle_anim(direction facing);
    bn::sprite_animate_action<4> _make_hurt_anim(direction facing);

    bn::sprite_ptr _sprite;
    bn::sprite_tiles_item _tiles;

    State _state;
    direction _dir;
    AttackCombo _combo;

    bn::optional<bn::sprite_animate_action<10>> _idle_anim;
    bn::optional<bn::sprite_animate_action<4>>  _walk_anim;
    bn::optional<bn::sprite_animate_action<4>>  _attack_anim;
    bn::optional<bn::sprite_animate_action<4>>  _hurt_anim;
};

#endif // GOKU_SPRITE_H
