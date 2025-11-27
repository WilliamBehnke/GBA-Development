#ifndef SNAKE_SPRITE_H
#define SNAKE_SPRITE_H

#include "sprite/BaseSprite.h"
#include "bn_sprite_items_snake.h"

#define IDLE(base)  { { base, base }, 2, 12, true }
#define WALK(base)  { { base, base + 1, base + 2, base + 3 }, 4, 6, true }
#define HURT(base)  { { base, base + 1, base + 2 }, 3, 6, false }
#define DEATH(base) { { base, base + 1, base + 2, base + 3 }, 4, 6, false }
#define ATK(base)   \
    {{ \
        { { base, base + 1, base + 2, base + 3 }, 4, 6, false } \
    }}

#define DIR(IDLE_BLOCK, WALK_BLOCK, HURT_BLOCK, DEATH_BLOCK, ATK_BLOCK) \
    { IDLE_BLOCK, WALK_BLOCK, HURT_BLOCK, DEATH_BLOCK, ATK_BLOCK }


class SnakeSprite : public BaseSprite {
public:
    SnakeSprite(int x, int y) :
        BaseSprite(
            bn::sprite_items::snake.tiles_item(),
            bn::sprite_items::snake.create_sprite(x, y),
            _anim_data,
            1
        )
    {}

private:
    static constexpr bn::array<DirectionSet, 4> _anim_data = {{
        DIR(IDLE(0), WALK(4),  HURT(36), DEATH(48), ATK(20)),
        DIR(IDLE(1), WALK(8),  HURT(39), DEATH(48), ATK(24)),
        DIR(IDLE(2), WALK(12), HURT(42), DEATH(48), ATK(28)),
        DIR(IDLE(3), WALK(16), HURT(45), DEATH(48), ATK(32))
    }};
};

#endif // SNAKE_SPRITE_H
