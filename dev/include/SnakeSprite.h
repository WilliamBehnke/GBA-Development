#ifndef SNAKE_SPRITE_H
#define SNAKE_SPRITE_H

#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_optional.h"

#include "bn_sprite_items_snake.h"
#include "CharacterSprite.h"

// Snake sprite that uses a sheet with:
//   - 1 idle frame / direction
//   - 4 walk frames / direction
//   - 4 attack frames / direction
//   - 3 hurt frames / direction
// Layout order for each block: UP, RIGHT, LEFT, DOWN.
class SnakeSprite : public CharacterSprite
{
public:
    explicit SnakeSprite(const bn::fixed_point& spawn_pos);

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

    // Per-frame update: handles patrol + animations.
    void update() override;

private:
    enum class State
    {
        Idle,
        Walk,
        Attack,
        Hurt
    };

    // Internal helpers -------------------------------------------------
    static int _direction_index(direction dir);   // UP, RIGHT, LEFT, DOWN -> 0..3

    // NOTE: Butano requires MaxSize > 1, so idle uses <2> and repeats the same frame.
    bn::sprite_animate_action<2>  _make_idle_anim(direction facing);
    bn::sprite_animate_action<4>  _make_walk_anim(direction facing);
    bn::sprite_animate_action<4>  _make_attack_anim(direction facing);
    bn::sprite_animate_action<3>  _make_hurt_anim(direction facing);

    bn::sprite_ptr _sprite;
    bn::sprite_tiles_item _tiles;

    State _state;
    direction _dir;

    // Simple patrol logic
    bn::fixed _speed;
    int _horizontal_dir;          // +1 = right, -1 = left

    bn::optional<bn::sprite_animate_action<2>> _idle_anim;
    bn::optional<bn::sprite_animate_action<4>> _walk_anim;
    bn::optional<bn::sprite_animate_action<4>> _attack_anim;
    bn::optional<bn::sprite_animate_action<3>> _hurt_anim;
};

#endif // SNAKE_SPRITE_H
