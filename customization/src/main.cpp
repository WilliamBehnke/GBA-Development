// ---------------------------------------------------------------------------
// main.cpp
// Entry point: runs the character customization screen.
// ---------------------------------------------------------------------------

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_map_cell.h"

#include "customization_screen.h"
#include "player.h"
#include "world_map.h"

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


    // 2) Create camera
    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    // 3) Create world + attach camera
    WorldMap world;
    world.set_camera(camera);

    // 4) Create player + attach same camera
    Player player(appearance, bn::fixed_point(0, 0));
    player.attach_camera(camera);

    while(true)
    {
        player.update(world);
        bn::core::update();
    }

    return 0;
}
