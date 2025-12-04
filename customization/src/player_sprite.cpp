#include "player_sprite.h"

#include "bn_sprite_palette_ptr.h"

PlayerSprite::PlayerSprite(const CharacterAppearance& appearance) :
    _appearance(appearance)
{
}

void PlayerSprite::rebuild(const bn::fixed_point& pos)
{
    _rebuild_sprites(pos);
    _apply_colors();
    _sync_sprites(pos);
}

void PlayerSprite::set_scale(int scale)
{
    if(_body_sprite)
        _body_sprite->set_scale(scale);
    if(_eyes_sprite)
        _eyes_sprite->set_scale(scale);
    if(_top_sprite)
        _top_sprite->set_scale(scale);
    if(_bottom_sprite)
        _bottom_sprite->set_scale(scale);
    if(_hair_sprite)
        _hair_sprite->set_scale(scale);
}

void PlayerSprite::attach_camera(const bn::camera_ptr& camera)
{
    _camera = camera;
    attach_camera();
}

void PlayerSprite::attach_camera()
{
    if(!_camera)
        return;

    auto& cam = _camera.value();
    if(_body_sprite)
        _body_sprite->set_camera(cam);
    if(_eyes_sprite)
        _eyes_sprite->set_camera(cam);
    if(_top_sprite)
        _top_sprite->set_camera(cam);
    if(_bottom_sprite)
        _bottom_sprite->set_camera(cam);
    if(_hair_sprite)
        _hair_sprite->set_camera(cam);
}

void PlayerSprite::detach_camera()
{
    if(!_camera)
        return;

    _camera.reset();
    if(_body_sprite)
        _body_sprite->remove_camera();
    if(_eyes_sprite)
        _eyes_sprite->remove_camera();
    if(_top_sprite)
        _top_sprite->remove_camera();
    if(_bottom_sprite)
        _bottom_sprite->remove_camera();
    if(_hair_sprite)
        _hair_sprite->remove_camera();
}

void PlayerSprite::update(const bn::fixed_point& pos,
                          FacingDirection direction,
                          bool moving)
{
    _direction = direction;
    _update_animation(moving);
    _sync_sprites(pos);
}

void PlayerSprite::_rebuild_sprites(const bn::fixed_point& pos)
{
    // Pick sprite_items based on indices in appearance
    _body_item   = k_body_type_options[0];
    _hair_item   = k_hair_options[_appearance.hair_index];
    _eyes_item   = k_eyes_options[0];
    _top_item    = k_top_options[_appearance.top_index];
    _bottom_item = k_bottom_options[_appearance.bottom_index];

    // Create sprites at current position
    _body_sprite   = _body_item->create_sprite(pos);
    _eyes_sprite   = _eyes_item->create_sprite(pos);
    _top_sprite    = _top_item->create_sprite(pos);
    _bottom_sprite = _bottom_item->create_sprite(pos);
    _hair_sprite   = _hair_item->create_sprite(pos);

    // Attach camera if it already exists
    attach_camera();

    // Set z-order (body -> eyes -> hair -> bottom -> top)
    _body_sprite->set_z_order(4);
    _eyes_sprite->set_z_order(3);
    _hair_sprite->set_z_order(2);
    _bottom_sprite->set_z_order(1);
    _top_sprite->set_z_order(0);

    // Set background order
    _body_sprite->set_bg_priority(1);
    _eyes_sprite->set_bg_priority(1);
    _top_sprite->set_bg_priority(1);
    _bottom_sprite->set_bg_priority(1);
    _hair_sprite->set_bg_priority(1);
}

void PlayerSprite::_apply_colors()
{
    // Skin
    {
        bn::sprite_palette_ptr pal = _body_sprite->palette();
        const ColorRamp& ramp = get_skin_ramp(_appearance.body_color);
        ramp.apply_ramp_to_palette(pal);
    }

    // Hair
    {
        bn::sprite_palette_ptr pal = _hair_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.hair_color);
        ramp.apply_ramp_to_palette(pal);
    }

    // Eyes
    {
        bn::sprite_palette_ptr pal = _eyes_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.eyes_color);
        ramp.apply_ramp_to_palette(pal);
    }

    // Top
    {
        bn::sprite_palette_ptr pal = _top_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.top_color);
        ramp.apply_ramp_to_palette(pal);
    }

    // Bottom
    {
        bn::sprite_palette_ptr pal = _bottom_sprite->palette();
        const ColorRamp& ramp = get_feature_ramp(_appearance.bottom_color);
        ramp.apply_ramp_to_palette(pal);
    }
}

void PlayerSprite::_update_animation(bool moving)
{
    // Layout per direction:
    // [idle0, idle1, walk0, walk1, walk2, walk3]
    constexpr int k_idle_period = 24;  // frames between flips when idle
    constexpr int k_walk_period = 6;   // frames between steps when walking

    _moving = moving;
    ++_anim_counter;

    if(_moving)
    {
        if(_anim_counter >= k_walk_period)
        {
            _anim_counter = 0;
            _walk_frame = (_walk_frame + 1) % 4;   // 0..3
        }
    }
    else
    {
        if(_anim_counter >= k_idle_period)
        {
            _anim_counter = 0;
            _idle_frame ^= 1;                      // toggle 0 <-> 1
        }

        // Reset walk cycle when you stop moving
        _walk_frame = 0;
    }
}

void PlayerSprite::_sync_sprites(const bn::fixed_point& pos)
{
    if(!_body_sprite || !_eyes_sprite || !_top_sprite || !_bottom_sprite || !_hair_sprite)
    {
        return;
    }

    // Position
    _body_sprite->set_position(pos);
    _eyes_sprite->set_position(pos);
    _top_sprite->set_position(pos);
    _bottom_sprite->set_position(pos);
    _hair_sprite->set_position(pos);

    // Apply camera if present
    attach_camera();

    // Frames per direction:
    // [idle0, idle1, walk0, walk1, walk2, walk3]
    constexpr int k_frames_per_direction = 6;

    // 0=Down, 1=Right, 2=Up, 3=Left (must match FacingDirection)
    int dir = static_cast<int>(_direction);
    int base_frame = dir * k_frames_per_direction;

    int frame_index;
    if(_moving)
    {
        frame_index = base_frame + 2 + _walk_frame;   // walk0..3
    }
    else
    {
        frame_index = base_frame + _idle_frame;       // idle0..1
    }

    _body_sprite->set_tiles(_body_item->tiles_item(), frame_index);
    _eyes_sprite->set_tiles(_eyes_item->tiles_item(), frame_index);
    _hair_sprite->set_tiles(_hair_item->tiles_item(), frame_index);
    _bottom_sprite->set_tiles(_bottom_item->tiles_item(), frame_index);
    _top_sprite->set_tiles(_top_item->tiles_item(), frame_index);
}
