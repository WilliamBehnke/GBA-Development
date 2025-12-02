// ---------------------------------------------------------------------------
// main.cpp
// Entry point: runs the character customization screen.
// ---------------------------------------------------------------------------

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"

#include "customization_screen.h"
#include "player.h"

int main()
{
    bn::core::init();

    // Set a neutral background
    bn::bg_palettes::set_transparent_color(bn::color(10, 10, 10));

    // -----------------------------
    // 1) Character customization
    // -----------------------------
    CharacterAppearance appearance;

    {
        CustomizationScreen customization;

        while(!customization.done())
        {
            customization.update();
            bn::core::update();
        }

        // Grab chosen appearance
        appearance = customization.appearance();
    }

    // -----------------------------
    // 2) In-game world with player
    // -----------------------------
    // Start at center of screen
    Player player(appearance, bn::fixed_point(0, 0));

    while(true)
    {
        player.update();
        bn::core::update();
    }

    return 0;
}
