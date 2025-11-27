#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_text_generator.h"

#include "common_fixed_8x8_sprite_font.h"

#include "entity/Player.h"
#include "entity/Enemy.h"
#include "entity/EntityManager.h"
#include "sprite/GokuSprite.h"
#include "sprite/SnakeSprite.h"
#include "ui/DamageNumbers.h"

int main()
{
    bn::core::init();

    // Set backdrop to light grey
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    // Damage numbers text generator
    bn::sprite_text_generator text_gen(common::fixed_8x8_sprite_font);
    DamageNumbers::initialize(&text_gen);

    // --------------------------------------------------
    // Entities
    // --------------------------------------------------
    GokuSprite goku_sprite(-80, 0);
    Player player(&goku_sprite);

    SnakeSprite snake1(80, 40);
    Enemy enemy1(&snake1);
    enemy1.set_target(&player);

    SnakeSprite snake2(80, -40);
    Enemy enemy2(&snake2);
    enemy2.set_target(&player);

    EntityManager entity_manager(&player);

    entity_manager.add_enemy(&enemy1);
    entity_manager.add_enemy(&enemy2);

    // -----------------------------------------
    // Main loop
    // -----------------------------------------
    while(true)
    {
        // Update all entities + attacks + bumping
        entity_manager.update();

        // Update floating damage numbers
        DamageNumbers::update();

        bn::core::update();
    }

    return 0;
}
