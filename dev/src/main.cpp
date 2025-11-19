#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_math.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_optional.h"
#include "bn_color.h"
#include "bn_bg_palettes.h"

#include "bn_sprite_items_hero.h"

namespace {
    constexpr bn::fixed k_speed = 1.5;
    constexpr bn::fixed k_half_width  = 120;   // 240 / 2
    constexpr bn::fixed k_half_height = 80;    // 160 / 2

    enum class direction {
        DOWN, RIGHT, LEFT, UP
    };
}

int main() {
    bn::core::init();

    // Set backdrop to light grey
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    // Start with idle facing down in the center
    bn::sprite_ptr hero = bn::sprite_items::hero.create_sprite(0, 0);

    bn::optional<bn::sprite_animate_action<10>> idle_anim;
    bn::optional<bn::sprite_animate_action<4>> walk_anim;

    direction facing = direction::DOWN;
    direction last_dir = facing;
    bool was_moving = false;

    const auto hero_tiles = bn::sprite_items::hero.tiles_item();

    while (true) {
        bn::fixed dx = 0;
        bn::fixed dy = 0;
        bool moving = false;

        // Read input + update facing
        if (bn::keypad::left_held()) {
            dx -= k_speed;
            moving = true;
            facing = direction::LEFT;
        }
        if (bn::keypad::right_held()) {
            dx += k_speed;
            moving = true;
            facing = direction::RIGHT;
        }
        if (bn::keypad::up_held()) {
            dy -= k_speed;
            moving = true;
            facing = direction::UP;
        }
        if (bn::keypad::down_held()) {
            dy += k_speed;
            moving = true;
            facing = direction::DOWN;
        }

        // Normalize diagonal movement
        if (moving && (dx != 0 || dy != 0)) {
            const bn::fixed inv_len = bn::fixed(1) / bn::sqrt(dx * dx + dy * dy);
            dx *= inv_len * k_speed;
            dy *= inv_len * k_speed;
        }

        // Update position
        bn::fixed_point pos = hero.position();
        pos.set_x(pos.x() + dx);
        pos.set_y(pos.y() + dy);

        // Clamp to screen bounds
        if (pos.x() < -k_half_width) {
            pos.set_x(-k_half_width);
        }
        else if (pos.x() > k_half_width) {
            pos.set_x(k_half_width);
        }

        if (pos.y() < -k_half_height) {
            pos.set_y(-k_half_height);
        }
        else if (pos.y() > k_half_height) {
            pos.set_y(k_half_height);
        }

        hero.set_position(pos);

        // --- ANIMATION SELECTION ---

        const bool direction_changed = (facing != last_dir);
        last_dir = facing;

        if (moving) {
            // Use walking animation
            if (!walk_anim.has_value() || !was_moving || direction_changed) {
                idle_anim.reset();

                switch(facing) {
                case direction::DOWN:
                    // walk down: frames 7–10
                    walk_anim = bn::create_sprite_animate_action_forever(
                        hero, 6, hero_tiles, 7, 8, 9, 10);
                    break;

                case direction::RIGHT:
                    // walk right: frames 11–14
                    walk_anim = bn::create_sprite_animate_action_forever(
                        hero, 6, hero_tiles, 11, 12, 13, 14);
                    break;

                case direction::LEFT:
                    // walk left: frames 15–18
                    walk_anim = bn::create_sprite_animate_action_forever(
                        hero, 6, hero_tiles, 15, 16, 17, 18);
                    break;

                case direction::UP:
                    // walk up: frames 19–22
                    walk_anim = bn::create_sprite_animate_action_forever(
                        hero, 6, hero_tiles, 19, 20, 21, 22);
                    break;

                default:
                    // Should never happen
                    return {};
                }
            }
        }
        else {
            // Use idle animation
            if (!idle_anim.has_value() || was_moving || direction_changed) {
                walk_anim.reset();

                switch(facing) {
                case direction::DOWN:
                    // idle down: frames 0–1 (mostly 0, blink to 1)
                    idle_anim = bn::create_sprite_animate_action_forever(
                        hero, 12, hero_tiles,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
                    break;

                case direction::RIGHT:
                    // idle right: frames 2–3
                    idle_anim = bn::create_sprite_animate_action_forever(
                        hero, 12, hero_tiles,
                        2, 2, 2, 2, 2, 2, 2, 2, 2, 3);
                    break;

                case direction::LEFT:
                    // idle left: frames 4–5
                    idle_anim = bn::create_sprite_animate_action_forever(
                        hero, 12, hero_tiles,
                        4, 4, 4, 4, 4, 4, 4, 4, 4, 5);
                    break;

                case direction::UP:
                    // idle up: frame 6 held
                    idle_anim = bn::create_sprite_animate_action_forever(
                        hero, 12, hero_tiles,
                        6, 6, 6, 6, 6, 6, 6, 6, 6, 6);
                    break;

                default:
                    // Should never happen
                    return {};
                }
            }
        }

        // Update whichever animation is active
        if (idle_anim.has_value()) {
            idle_anim->update();
        }
        if (walk_anim.has_value()) {
            walk_anim->update();
        }

        was_moving = moving;

        bn::core::update(); // Advance one frame
    }

    return 0;
}
