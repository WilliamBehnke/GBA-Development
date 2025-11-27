#ifndef GOKU_SPRITE_H
#define GOKU_SPRITE_H

#include "sprite/BaseSprite.h"
#include "bn_sprite_items_goku.h"

#define IDLE1(main)               IDLE10(main, main)
#define IDLE2(main, blink)        IDLE10(main, blink)
#define GET_MACRO(_1,_2,NAME,...) NAME
#define IDLE(...)                 GET_MACRO(__VA_ARGS__, IDLE2, IDLE1)(__VA_ARGS__)
#define IDLE10(main, blink) \
    { { main, main, main, main, main, main, main, main, main, blink }, 10, 12, true }

#define WALK(base) { { base, base + 1, base + 2, base + 3 }, 4, 6, true }
#define HURT(base) { { base, base + 1, base + 2, base + 3 }, 4, 6, false }
#define DEATH(base) { { base, base + 1, base + 2, base + 3 }, 4, 6, false, true }
#define ATK(a,b,c) \
    {{ \
        { { a, a + 1, a + 2, a + 3 }, 4, 6, false }, \
        { { b, b + 1, b + 2, b + 3 }, 4, 6, false }, \
        { { c, c + 1, c + 2, c + 3 }, 4, 6, false }, \
    }}

#define DIR(IDLE_BLOCK, WALK_BLOCK, HURT_BLOCK, DEATH_BLOCK, ATK_BLOCK) \
    { IDLE_BLOCK, WALK_BLOCK, HURT_BLOCK, DEATH_BLOCK, ATK_BLOCK }


class GokuSprite : public BaseSprite
{
public:
    GokuSprite(int x, int y) :
        BaseSprite(
            bn::sprite_items::goku.tiles_item(),
            bn::sprite_items::goku.create_sprite(x, y),
            _anim_data,
            3
        )
    {}

private:
    static constexpr bn::array<DirectionSet, 4> _anim_data = {{
        DIR(IDLE(0,1), WALK(7),  HURT(71), DEATH(87), ATK(23,27,31)),
        DIR(IDLE(2,3), WALK(11), HURT(75), DEATH(87), ATK(35,39,43)),
        DIR(IDLE(4,5), WALK(15), HURT(79), DEATH(87), ATK(47,51,55)),
        DIR(IDLE(6),   WALK(19), HURT(83), DEATH(87), ATK(59,63,67))
    }};
};

#endif // GOKU_SPRITE_H
