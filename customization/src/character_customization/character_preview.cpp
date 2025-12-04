// ---------------------------------------------------------------------------
// character_preview.cpp
// ---------------------------------------------------------------------------

#include "character_preview.h"

#include "bn_sprite_items_character_border.h"

CharacterPreview::CharacterPreview(const bn::fixed_point& pos,
                                   const CharacterAppearance& appearance) :
    _pos(pos),
    _direction(appearance.direction),
    _appearance(appearance),
    _sprite(_appearance),
    _moving(false),
    _border(bn::sprite_items::character_border.create_sprite(pos))
{
    if(_border)
    {
        _border->set_z_order(5);      // on top of character layers
        _border->set_bg_priority(1);
    }

    // Build the character sprites once at construction
    _sprite.rebuild(_pos);
}

void CharacterPreview::set_direction(FacingDirection dir)
{
    _direction = dir;
}

void CharacterPreview::toggle_animation()
{
    _moving = !_moving;
}

void CharacterPreview::refresh()
{
    _sprite.rebuild(_pos);
}

void CharacterPreview::set_scale(int scale)
{
    _sprite.set_scale(scale);
}

void CharacterPreview::set_position(const bn::fixed_point& pos)
{
    _pos = pos;

    if(_border)
    {
        _border->set_position(_pos);
    }

    _sprite.update(_pos, _direction, _moving);
}

void CharacterPreview::update()
{
    _sprite.update(_pos, _direction, _moving);
}
