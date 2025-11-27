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
    // Called once in main(), stores a pointer to the text generator
    static void initialize(bn::sprite_text_generator* gen)
    {
        _text_gen = gen;
        if(_text_gen)
            _text_gen->set_center_alignment();
    }

    // Create a floating damage popup
    static void spawn(const bn::fixed_point& pos, int amount)
    {
        if(!_text_gen || _entries.full())
            return;

        Entry e;
        e.pos = pos;
        e.velocity = bn::fixed_point(0, -0.4);
        e.lifetime = 30;

        bn::string<8> text;
        //text += "-";
        text += bn::to_string<6>(amount);

        _text_gen->generate(
            pos.x().integer(),
            pos.y().integer(),
            text,
            e.sprites
        );

        _entries.push_back(e);
    }

    // Called once per frame from the main loop
    static void update()
    {
        for(int i = _entries.size() - 1; i >= 0; --i)
        {
            Entry& e = _entries[i];

            // Move upward
            e.pos += e.velocity;

            // Reposition sprites
            for(int j = 0; j < e.sprites.size(); j++)
            {
                bn::fixed x = e.pos.x() + bn::fixed(8 * (j - e.sprites.size() / 2));
                bn::fixed y = e.pos.y();
                e.sprites[j].set_position(x, y);
                e.sprites[j].set_z_order(0);
            }

            // Fade out time
            --e.lifetime;
            if(e.lifetime <= 0)
            {
                e.sprites.clear();
                _entries.erase(_entries.begin() + i);
            }
        }
    }

private:
    struct Entry
    {
        bn::fixed_point pos;
        bn::fixed_point velocity;
        int lifetime = 30;
        bn::vector<bn::sprite_ptr, 8> sprites;
    };

    static inline bn::sprite_text_generator* _text_gen = nullptr;
    static inline bn::vector<Entry, 16> _entries;
};

#endif // DAMAGE_NUMBERS_H
