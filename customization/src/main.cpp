// ---------------------------------------------------------------------------
// main.cpp
// Entry point: runs the character customization screen.
// ---------------------------------------------------------------------------

#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_color.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_regular_bg_items_bg.h"
#include "bn_sprite_text_generator.h"

#include "common_fixed_8x8_sprite_font.h"

#include "customization_screen.h"
#include "player.h"
#include "enemy.h"
#include "enemy_sprite.h"
#include "entity_manager.h"
#include "world_map.h"
#include "damage_numbers.h"

void update_camera(bn::camera_ptr& camera, WorldMap* world, bn::fixed_point& pos)
{
    int map_px_w = world->pixel_width();
    int map_px_h = world->pixel_height();

    const bn::fixed half_w = 120;   // 240 / 2
    const bn::fixed half_h = 80;    // 160 / 2

    bn::fixed cx = pos.x();
    bn::fixed cy = pos.y();

    bn::fixed min_x = bn::fixed(-map_px_w / 2) + half_w;
    bn::fixed max_x = bn::fixed( map_px_w / 2) - half_w;
    bn::fixed min_y = bn::fixed(-map_px_h / 2) + half_h;
    bn::fixed max_y = bn::fixed( map_px_h / 2) - half_h;

    if(min_x > max_x)
        cx = 0;
    else
        cx = bn::clamp(cx, min_x, max_x);

    if(min_y > max_y)
        cy = 0;
    else
        cy = bn::clamp(cy, min_y, max_y);

    camera.set_x(cx);
    camera.set_y(cy);
}

int main()
{
    bn::core::init();

    // -----------------------------
    // 1) Character customization
    // -----------------------------
    CharacterAppearance appearance;

    {
        bn::regular_bg_ptr _bg = bn::regular_bg_items::bg.create_bg(0, 0);

        CustomizationScreen customization;

        while(!customization.done())
        {
            customization.update();
            bn::core::update();
        }

        // Grab chosen appearance
        appearance = customization.appearance();
    }

    // Set a neutral background
    bn::bg_palettes::set_transparent_color(bn::color(10, 10, 10));

    // 2) Create camera
    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    // Damage numbers text generator
    bn::sprite_text_generator text_gen(common::fixed_8x8_sprite_font);
    DamageNumbers::initialize(&text_gen, &camera);

    // 3) Create world + attach camera
    WorldMap* world = new WorldMap(RoomId::MainRoom);
    world->set_camera(camera);

    // 4) Create player + attach same camera
    PlayerSprite player_sprite(appearance);
    Player player(&player_sprite, bn::fixed_point(0, 0), world);
    player.attach_camera(camera);

    EntityManager entity_manager(&player, RoomId::MainRoom);

    EnemySprite enemy_sprite1(bn::fixed_point(-200, 0));
    Enemy enemy1(&enemy_sprite1, world);
    enemy1.attach_camera(camera);
    enemy1.set_target(&player);
    entity_manager.add_enemy(&enemy1, RoomId::MainRoom);

    EnemySprite enemy_sprite2(bn::fixed_point(0, -150));
    Enemy enemy2(&enemy_sprite2, world);
    enemy2.attach_camera(camera);
    enemy2.set_target(&player);
    entity_manager.add_enemy(&enemy2, RoomId::MainRoom);

    EnemySprite enemy_sprite3(bn::fixed_point(50, 200));
    Enemy enemy3(&enemy_sprite3, world);
    enemy3.attach_camera(camera);
    enemy3.set_target(&player);
    entity_manager.add_enemy(&enemy3, RoomId::MainRoom);

    while(true)
    {
        // 1) Normal updates
        player.update();
        entity_manager.update();
        world->update();

        // 2) Check for door collision using the player's position
        if(auto door = world->check_door_collision(player_sprite.position()))
        {
            // Copy out values BEFORE changing the room, so we don't use a dangling pointer
            RoomId target_room = door->room_id;
            bn::fixed_point spawn_pos = door->spawn_pos;

            // --- Fade out ---------------------------------------------------
            for(int i = 0; i <= 16; ++i)
            {
                bn::fixed intensity = bn::fixed(i) / 16;

                bn::bg_palettes::set_fade(bn::color(0, 0, 0), intensity);
                bn::sprite_palettes::set_fade(bn::color(0, 0, 0), intensity);

                bn::core::update();
            }

            // --- Actually change the room ----------------------------------
            world->change_room(target_room);
            entity_manager.set_current_room(target_room);

            // Teleport player to the door's spawn position
            player.update_sprite(spawn_pos, FacingDirection::Down);

            // Recenter camera on the player and re-attach to world
            update_camera(camera, world, spawn_pos);
            world->set_camera(camera);

            // --- Fade back in ----------------------------------------------
            for(int i = 16; i >= 0; --i)
            {
                bn::fixed intensity = bn::fixed(i) / 16;

                bn::bg_palettes::set_fade(bn::color(0, 0, 0), intensity);
                bn::sprite_palettes::set_fade(bn::color(0, 0, 0), intensity);

                bn::core::update();
            }
        }

        DamageNumbers::update();

        bn::core::update();
    }

    delete world;

    return 0;
}
