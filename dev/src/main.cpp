#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_math.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"

#include "bn_sprite_items_hero.h"

int main() {
    bn::core::init();

    // Start our sprite in the middle of the screen
    bn::sprite_ptr hero = bn::sprite_items::hero.create_sprite(0, 0);

    // Movement speed in pixels per frame (fractional)
    const bn::fixed speed = 1.0;

    while(true) {
        bn::fixed dx = 0;
        bn::fixed dy = 0;

        // Read input
        if(bn::keypad::left_held()) {
            dx -= speed;
        }
        if(bn::keypad::right_held()) {
            dx += speed;
        }
        if(bn::keypad::up_held()) {
            dy -= speed;
        }
        if(bn::keypad::down_held()) {
            dy += speed;
        }

        // If moving diagonally, normalize so you don't move faster on diagonals
        if(dx != 0 || dy != 0) {
            bn::fixed inv_len = bn::fixed(1) / bn::sqrt(dx * dx + dy * dy);
            dx *= inv_len * speed;
            dy *= inv_len * speed;
        }

        // Update position
        bn::fixed_point pos = hero.position();
        pos.set_x(pos.x() + dx);
        pos.set_y(pos.y() + dy);
        hero.set_position(pos);

        // Clamp to screen bounds (approx. visible area)
        const bn::fixed half_width  = 120;   // 240 / 2
        const bn::fixed half_height = 80;    // 160 / 2

        if(pos.x() < -half_width) {
            pos.set_x(-half_width);
        }
        else if(pos.x() > half_width) {
            pos.set_x(half_width);
        }

        if(pos.y() < -half_height) {
            pos.set_y(-half_height);
        }
        else if(pos.y() > half_height) {
            pos.set_y(half_height);
        }

        hero.set_position(pos);

        bn::core::update(); // Advance one frame
    }

    return 0;
}
