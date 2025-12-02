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
    void apply_ramp_to_palette(const ColorRamp& ramp, bn::sprite_palette_ptr& pal)
    {
        pal.set_color(1, ramp.c0);
        pal.set_color(2, ramp.c1);
        pal.set_color(3, ramp.c2);
        pal.set_color(4, ramp.c3);
    }
}

CharacterPreview::CharacterPreview(const bn::fixed_point& pos) :
    _pos(pos)
{
    // Sprites will be created on first set_layers call
}

// ---------------------------------------------------------------------------

void CharacterPreview::set_direction(FacingDirection dir)
{
    _direction = dir;
}

// ---------------------------------------------------------------------------

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

    _rebuild_sprites();
    _sync_sprites();
}

// ---------------------------------------------------------------------------

void CharacterPreview::apply_colors(const CharacterAppearance& appearance)
{
    // Body (skin)
    if(_body_sprite)
    {
        bn::sprite_palette_ptr pal = _body_sprite->palette();
        const ColorRamp& ramp = get_skin_ramp(appearance.body_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Hair
    if(_hair_sprite)
    {
        bn::sprite_palette_ptr pal = _hair_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(appearance.hair_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Eyes
    if(_eyes_sprite)
    {
        bn::sprite_palette_ptr pal = _eyes_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(appearance.eyes_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Top
    if(_top_sprite)
    {
        bn::sprite_palette_ptr pal = _top_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(appearance.top_color_index);
        apply_ramp_to_palette(ramp, pal);
    }

    // Bottom
    if(_bottom_sprite)
    {
        bn::sprite_palette_ptr pal = _bottom_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(appearance.bottom_color_index);
        apply_ramp_to_palette(ramp, pal);
    }
}

// ---------------------------------------------------------------------------

void CharacterPreview::set_scale(int scale)
{
    if(_body_sprite)
    {
        _body_sprite->set_scale(scale);
    }
    if(_eyes_sprite)
    {
        _eyes_sprite->set_scale(scale);
    }
    if(_top_sprite)
    {
        _top_sprite->set_scale(scale);
    }
    if(_bottom_sprite)
    {
        _bottom_sprite->set_scale(scale);
    }
    if(_hair_sprite)
    {
        _hair_sprite->set_scale(scale);
    }
}

// ---------------------------------------------------------------------------

void CharacterPreview::set_position(const bn::fixed_point& pos)
{
    _pos = pos;
    _sync_sprites();
}

// ---------------------------------------------------------------------------

void CharacterPreview::_rebuild_sprites()
{
    // Destroy old sprites by overwriting optionals
    if(_body_item)
    {
        _body_sprite = _body_item->create_sprite(_pos);
    }
    if(_eyes_item)
    {
        _eyes_sprite = _eyes_item->create_sprite(_pos);
    }
    if(_top_item)
    {
        _top_sprite = _top_item->create_sprite(_pos);
    }
    if(_bottom_item)
    {
        _bottom_sprite = _bottom_item->create_sprite(_pos);
    }
    if(_hair_item)
    {
        _hair_sprite = _hair_item->create_sprite(_pos);
    }

    if(_body_sprite)   _body_sprite->set_z_order(4);
    if(_eyes_sprite)   _eyes_sprite->set_z_order(3);
    if(_hair_sprite)   _hair_sprite->set_z_order(2);
    if(_bottom_sprite) _bottom_sprite->set_z_order(1);
    if(_top_sprite)    _top_sprite->set_z_order(0);
}

// ---------------------------------------------------------------------------

void CharacterPreview::_sync_sprites()
{
    // Position
    if(_body_sprite)   _body_sprite->set_position(_pos);
    if(_eyes_sprite)   _eyes_sprite->set_position(_pos);
    if(_top_sprite)    _top_sprite->set_position(_pos);
    if(_bottom_sprite) _bottom_sprite->set_position(_pos);
    if(_hair_sprite)   _hair_sprite->set_position(_pos);

    // Frames:
    // Your sheets have 6 frames per direction (idle0, idle1, walk0–3),
    // but for the preview we only use the first two (idle).
    constexpr int k_frames_per_direction = 6;

    int dir = static_cast<int>(_direction);      // 0=Down,1=Right,2=Up,3=Left
    int base_frame = dir * k_frames_per_direction;
    int frame_index = base_frame + _idle_frame;  // idle0 or idle1

    if(_body_sprite && _body_item)
        _body_sprite->set_tiles(_body_item->tiles_item(), frame_index);
    if(_eyes_sprite && _eyes_item)
        _eyes_sprite->set_tiles(_eyes_item->tiles_item(), frame_index);
    if(_top_sprite && _top_item)
        _top_sprite->set_tiles(_top_item->tiles_item(), frame_index);
    if(_bottom_sprite && _bottom_item)
        _bottom_sprite->set_tiles(_bottom_item->tiles_item(), frame_index);
    if(_hair_sprite && _hair_item)
        _hair_sprite->set_tiles(_hair_item->tiles_item(), frame_index);
}

// ---------------------------------------------------------------------------

void CharacterPreview::update()
{
    // Simple 2-frame idle bounce
    constexpr int k_idle_period = 24;    // tweak for speed

    ++_idle_counter;
    if(_idle_counter >= k_idle_period)
    {
        _idle_counter = 0;
        _idle_frame ^= 1;                // toggle 0 <-> 1
        _sync_sprites();
    }
}
