#include "bn_core.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"

#include "screen_manager.h"

int main()
{
    bn::core::init();

    // Optional: set background color
    bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));

    ScreenManager manager;

    while(true)
    {
        manager.update();
        bn::core::update();
    }
}
