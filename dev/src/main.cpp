#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"

#include "Player.h"
#include "GokuSprite.h"
#include "SnakeSprite.h"

int main()
{
    bn::core::init();

    // Set backdrop to light grey
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    // Player (Goku)
    GokuSprite goku_sprite;
    Player player(goku_sprite);

    // Snake enemy, patrolling around x=80
    SnakeSprite snake(bn::fixed_point(80, 0));

    while(true)
    {
        player.update();
        snake.update();

        bn::core::update(); // Advance one frame
    }

    return 0;
}
