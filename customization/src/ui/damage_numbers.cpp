#include "damage_numbers.h"

bn::sprite_text_generator* DamageNumbers::_text_gen = nullptr;
bn::camera_ptr* DamageNumbers::_camera = nullptr;
bn::vector<DamageNumbers::Entry, 16> DamageNumbers::_entries;

void DamageNumbers::initialize(bn::sprite_text_generator* gen, bn::camera_ptr* camera)
{
    _text_gen = gen;
    _camera = camera;

    if(_text_gen)
    {
        _text_gen->set_center_alignment();
    }
}

void DamageNumbers::spawn(const bn::fixed_point& pos, int amount)
{
    if(!_text_gen || _entries.full())
    {
        return;
    }

    Entry e;
    e.pos      = pos;
    e.velocity = bn::fixed_point(0, -0.4);
    e.lifetime = 30;

    bn::string<8> text;
    text += bn::to_string<6>(amount);

    _text_gen->generate(
        pos.x().integer(),
        pos.y().integer(),
        text,
        e.sprites
    );

    for(bn::sprite_ptr& s : e.sprites)
    {
        if(_camera)
        {
            s.set_camera(*_camera);
        }
        s.set_bg_priority(1);
    }

    _entries.push_back(e);
}

void DamageNumbers::update()
{
    for(int i = _entries.size() - 1; i >= 0; --i)
    {
        Entry& e = _entries[i];

        // Move upward
        e.pos += e.velocity;

        // Reposition sprites
        const int count = e.sprites.size();
        for(int j = 0; j < count; j++)
        {
            bn::fixed x = e.pos.x() + bn::fixed(8 * (j - count / 2));
            bn::fixed y = e.pos.y();

            e.sprites[j].set_position(x, y);
            e.sprites[j].set_z_order(-32767);   // Highest priority layer
        }

        // Fade out
        --e.lifetime;
        if(e.lifetime <= 0)
        {
            e.sprites.clear();
            _entries.erase(_entries.begin() + i);
        }
    }
}
