#ifndef DAMAGE_NUMBERS_H
#define DAMAGE_NUMBERS_H

#include "bn_fixed_point.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_string.h"

class DamageNumbers
{
public:
    // Initialize text generator reference
    static void initialize(bn::sprite_text_generator* gen);

    // Spawn floating damage popup
    static void spawn(const bn::fixed_point& pos, int amount);

    // Update all active damage popups
    static void update();

private:
    struct Entry
    {
        bn::fixed_point pos;
        bn::fixed_point velocity;
        int lifetime = 30;
        bn::vector<bn::sprite_ptr, 8> sprites;
    };

    static bn::sprite_text_generator* _text_gen;
    static bn::vector<Entry, 16> _entries;
};

#endif // DAMAGE_NUMBERS_H
