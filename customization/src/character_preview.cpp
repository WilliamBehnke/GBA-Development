// ---------------------------------------------------------------------------
// character_preview.cpp
// ---------------------------------------------------------------------------

#include "character_preview.h"

#include "bn_sprite_item.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_palette_ptr.h"

#include "character_colors.h"

namespace
{
    constexpr int k_frame_count       = 6;
    constexpr int k_anim_wait_updates = 16;  // lower = faster animation

    int _direction_row(FacingDirection dir)
    {
        switch(dir)
        {
            case FacingDirection::Down:  return 0;
            case FacingDirection::Left:  return 3;
            case FacingDirection::Up:    return 1;
            case FacingDirection::Right: return 2;
            default:                     return 0;
        }
    }

    void _apply_ramp_to_sprite(const ColorRamp& ramp,
                               bn::optional<bn::sprite_ptr>& sprite)
    {
        if(!sprite)
        {
            return;
        }

        // Each component sprite uses palette indices:
        // 0 = transparent, 1..4 = 4 shades
        bn::sprite_palette_ptr pal = sprite->palette();
        pal.set_color(1, ramp.c0);
        pal.set_color(2, ramp.c1);
        pal.set_color(3, ramp.c2);
        pal.set_color(4, ramp.c3);
    }
}

CharacterPreview::CharacterPreview(const bn::fixed_point& position) :
    _pos(position)
{
}

void CharacterPreview::set_layers(const bn::sprite_item* body,
                                  const bn::sprite_item* eyes,
                                  const bn::sprite_item* top,
                                  const bn::sprite_item* bottom,
                                  const bn::sprite_item* hair)
{
    _body_item   = body;
    _eyes_item   = eyes;
    _top_item    = top;
    _bottom_item = bottom;
    _hair_item   = hair;

    _rebuild_layers();
}

void CharacterPreview::set_direction(FacingDirection direction)
{
    if(direction == _direction)
    {
        return;
    }

    _direction = direction;
    _rebuild_layers();
}

void CharacterPreview::set_scale(int scale)
{
    if (_body_sprite)   _body_sprite->set_scale(scale);
    if (_eyes_sprite)   _eyes_sprite->set_scale(scale);
    if (_top_sprite)    _top_sprite->set_scale(scale);
    if (_bottom_sprite) _bottom_sprite->set_scale(scale);
    if (_hair_sprite)   _hair_sprite->set_scale(scale);
}

void CharacterPreview::_rebuild_layers()
{
    int row        = _direction_row(_direction);
    int base_frame = row * k_frame_count;

    auto setup_layer = [&](const bn::sprite_item* item,
                           bn::optional<bn::sprite_ptr>& sprite,
                           bn::optional<bn::sprite_animate_action<2>>& anim,
                           int z_order)
    {
        if(item)
        {
            sprite = item->create_sprite(_pos);
            sprite->set_z_order(z_order);

            anim = bn::create_sprite_animate_action_forever(
                sprite.value(),
                k_anim_wait_updates,
                item->tiles_item(),
                base_frame, base_frame + 1);   // 2-frame idle
        }
        else
        {
            sprite.reset();
            anim.reset();
        }
    };

    // BACK (drawn first) -------------------------- FRONT (drawn last)
    setup_layer(_body_item,   _body_sprite,   _body_anim,   4); // base/body at back
    setup_layer(_bottom_item, _bottom_sprite, _bottom_anim, 3);
    setup_layer(_top_item,    _top_sprite,    _top_anim,    2);
    setup_layer(_eyes_item,   _eyes_sprite,   _eyes_anim,   1);
    setup_layer(_hair_item,   _hair_sprite,   _hair_anim,   0); // hair on top
}

void CharacterPreview::apply_colors(const CharacterAppearance& appearance)
{
    // Body uses body color ramps (pale / tan / dark)
    const ColorRamp& body_ramp = get_skin_ramp(appearance.body_color_index);
    _apply_ramp_to_sprite(body_ramp, _body_sprite);

    // Other components use shared feature color ramps
    const ColorRamp& hair_ramp   = get_feature_ramp(appearance.hair_color_index);
    const ColorRamp& eyes_ramp   = get_feature_ramp(appearance.eyes_color_index);
    const ColorRamp& top_ramp    = get_feature_ramp(appearance.top_color_index);
    const ColorRamp& bottom_ramp = get_feature_ramp(appearance.bottom_color_index);

    _apply_ramp_to_sprite(hair_ramp,   _hair_sprite);
    _apply_ramp_to_sprite(eyes_ramp,   _eyes_sprite);
    _apply_ramp_to_sprite(top_ramp,    _top_sprite);
    _apply_ramp_to_sprite(bottom_ramp, _bottom_sprite);
}

void CharacterPreview::update()
{
    if(_body_anim)
        _body_anim->update();
    if(_bottom_anim)
        _bottom_anim->update();
    if(_top_anim)
        _top_anim->update();
    if(_eyes_anim)
        _eyes_anim->update();
    if(_hair_anim)
        _hair_anim->update();
}
