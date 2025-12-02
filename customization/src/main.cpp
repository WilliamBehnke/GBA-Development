// ---------------------------------------------------------------------------
// main.cpp
// Entry point: runs the character customization screen.
// ---------------------------------------------------------------------------

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"

#include "customization_screen.h"

int main()
{
    bn::core::init();

    // Optional: Set transparent/background color
    bn::bg_palettes::set_transparent_color(bn::color(10, 10, 10));

    CustomizationScreen customization_screen;

    while(true)
    {
        customization_screen.update();

        // Later:
        // if(A pressed) -> grab customization_screen.appearance()
        // and pass that into your Player entity / game state.

        bn::core::update();
    }
}
