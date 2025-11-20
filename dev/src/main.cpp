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
#include "bn_assert.h"

#include "bn_sprite_items_hero.h"

namespace
{
    constexpr bn::fixed k_speed = 1.5;
    constexpr bn::fixed k_half_width  = 120;   // 240 / 2
    constexpr bn::fixed k_half_height = 80;    // 160 / 2

    enum class direction
    {
        DOWN, RIGHT, LEFT, UP
    };

    enum class attack_combo
    {
        RIGHT_HOOK, LEFT_HOOK, KICK
    };

    constexpr attack_combo next(attack_combo c)
    {
        int i = (static_cast<int>(c) + 1) % 3;
        return static_cast<attack_combo>(i);
    }

    // -------------------------------------------------------------------------
    // Helper: clamp position to screen bounds
    // -------------------------------------------------------------------------
    void clamp_to_bounds(bn::fixed_point& pos)
    {
        if(pos.x() < -k_half_width)
        {
            pos.set_x(-k_half_width);
        }
        else if(pos.x() > k_half_width)
        {
            pos.set_x(k_half_width);
        }

        if(pos.y() < -k_half_height)
        {
            pos.set_y(-k_half_height);
        }
        else if(pos.y() > k_half_height)
        {
            pos.set_y(k_half_height);
        }
    }

    // -------------------------------------------------------------------------
    // Helper: attack animation (combo + direction)
    // attack tiles are laid out as:
    //   4 frames per combo
    //   3 combos per direction
    //   4 directions (DOWN, RIGHT, LEFT, UP)
    // -------------------------------------------------------------------------
    bn::sprite_animate_action<4> make_attack_anim(direction facing,
                                                  attack_combo combo,
                                                  bn::sprite_ptr& hero,
                                                  const bn::sprite_tiles_item& hero_tiles)
    {
        constexpr int combos_per_direction = 3;
        constexpr int frames_per_combo = 4;
        constexpr int down_attack_start = 23;

        int dir_index = static_cast<int>(facing);
        int combo_index = static_cast<int>(combo);
        int base = down_attack_start + (dir_index * combos_per_direction + combo_index) * frames_per_combo;

        return bn::create_sprite_animate_action_once(
            hero,
            6,
            hero_tiles,
            base, base + 1, base + 2, base + 3
        );
    }

    // -------------------------------------------------------------------------
    // Helper: walk animation (direction)
    // hero walk frames:
    //   DOWN :  7–10
    //   RIGHT: 11–14
    //   LEFT : 15–18
    //   UP   : 19–22
    // pattern: start = 7 + dir_index * 4
    // -------------------------------------------------------------------------
    bn::sprite_animate_action<4> make_walk_anim(direction facing,
                                                bn::sprite_ptr& hero,
                                                const bn::sprite_tiles_item& hero_tiles)
    {
        constexpr int frames_per_walk = 4;
        constexpr int down_walk_start = 7;

        int dir_index = static_cast<int>(facing);
        int base = down_walk_start + dir_index * frames_per_walk;

        return bn::create_sprite_animate_action_forever(
            hero,
            6,
            hero_tiles,
            base, base + 1, base + 2, base + 3
        );
    }

    // -------------------------------------------------------------------------
    // Helper: idle animation (direction)
    // hero idle frames:
    //   DOWN : main=0, blink=1
    //   RIGHT: main=2, blink=3
    //   LEFT : main=4, blink=5
    //   UP   : main=6 (no real blink; just held)
    //
    // Sequence: 9x main_frame, then blink_frame (or main for UP)
    // -------------------------------------------------------------------------
    bn::sprite_animate_action<10> make_idle_anim(direction facing,
                                                 bn::sprite_ptr& hero,
                                                 const bn::sprite_tiles_item& hero_tiles)
    {
        int main_frame = static_cast<int>(facing) * 2;
        int blink_frame = main_frame + 1;

        if(facing == direction::UP)  // No blink for when facing up
        {
            blink_frame = main_frame;
        }

        return bn::create_sprite_animate_action_forever(
            hero,
            12,
            hero_tiles,
            main_frame, main_frame, main_frame, main_frame, main_frame,
            main_frame, main_frame, main_frame, main_frame, blink_frame
        );
    }
}

int main()
{
    bn::core::init();

    // Set backdrop to light grey
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    // Start with idle facing down in the center
    bn::sprite_ptr hero = bn::sprite_items::hero.create_sprite(0, 0);

    bn::optional<bn::sprite_animate_action<10>> idle_anim;
    bn::optional<bn::sprite_animate_action<4>>  walk_anim;
    bn::optional<bn::sprite_animate_action<4>>  attack_anim;

    direction facing = direction::DOWN;
    direction last_dir = facing;
    bool was_moving = false;

    attack_combo combo = attack_combo::RIGHT_HOOK;

    const auto hero_tiles = bn::sprite_items::hero.tiles_item();

    while(true)
    {
        bn::fixed dx = 0;
        bn::fixed dy = 0;
        bool moving = false;

        const bool is_attacking = attack_anim.has_value();

        // ---------------------------------------------------------------------
        // MOVEMENT INPUT (but do not apply yet)
        // ---------------------------------------------------------------------
        if(!is_attacking)
        {
            if(bn::keypad::left_held())
            {
                dx -= k_speed;
                moving = true;
                facing = direction::LEFT;
            }
            if(bn::keypad::right_held())
            {
                dx += k_speed;
                moving = true;
                facing = direction::RIGHT;
            }
            if(bn::keypad::up_held())
            {
                dy -= k_speed;
                moving = true;
                facing = direction::UP;
            }
            if(bn::keypad::down_held())
            {
                dy += k_speed;
                moving = true;
                facing = direction::DOWN;
            }

            // Normalize diagonal movement
            if(moving && (dx != 0 || dy != 0))
            {
                const bn::fixed inv_len = bn::fixed(1) / bn::sqrt(dx * dx + dy * dy);
                dx *= inv_len * k_speed;
                dy *= inv_len * k_speed;
            }
        }

        // ---------------------------------------------------------------------
        // ATTACK INPUT (A button) – starts attack + combo
        // ---------------------------------------------------------------------
        if(!is_attacking && bn::keypad::a_pressed())
        {
            idle_anim.reset();
            walk_anim.reset();

            attack_anim = make_attack_anim(facing, combo, hero, hero_tiles);

            // Advance combo: 0 -> 1 -> 2 -> 0 ...
            combo = next(combo);

            // Cancel movement on the frame we start attacking
            dx = 0;
            dy = 0;
            moving = false;
        }

        // ---------------------------------------------------------------------
        // APPLY MOVEMENT (only if not attacking)
        // ---------------------------------------------------------------------
        if(!attack_anim.has_value())
        {
            bn::fixed_point pos = hero.position();
            pos.set_x(pos.x() + dx);
            pos.set_y(pos.y() + dy);

            clamp_to_bounds(pos);
            hero.set_position(pos);
        }

        // ---------------------------------------------------------------------
        // ANIMATION SELECTION (only when not attacking)
        // ---------------------------------------------------------------------
        const bool direction_changed = (facing != last_dir);
        last_dir = facing;

        if(!attack_anim.has_value())
        {
            if(moving)
            {
                // Use walking animation
                if(!walk_anim.has_value() || !was_moving || direction_changed)
                {
                    idle_anim.reset();
                    walk_anim = make_walk_anim(facing, hero, hero_tiles);
                }
            }
            else
            {
                // Use idle animation
                if(!idle_anim.has_value() || was_moving || direction_changed)
                {
                    walk_anim.reset();
                    idle_anim = make_idle_anim(facing, hero, hero_tiles);
                }
            }
        }

        // ---------------------------------------------------------------------
        // UPDATE ANIMATIONS
        // ---------------------------------------------------------------------
        if(attack_anim.has_value())
        {
            attack_anim->update();

            if(attack_anim->done())
            {
                attack_anim.reset();
                idle_anim = make_idle_anim(facing, hero, hero_tiles);
            }
        }
        else if(walk_anim.has_value())
        {
            walk_anim->update();
        }
        else if(idle_anim.has_value())
        {
            idle_anim->update();
        }

        was_moving = moving;

        bn::core::update(); // Advance one frame
    }

    return 0;
}
