// ---------------------------------------------------------------------------
// player_sprite.cpp
// ---------------------------------------------------------------------------

#include "player_sprite.h"

#include "bn_sprite_palette_ptr.h"

PlayerSprite::PlayerSprite(const CharacterAppearance& appearance) :
    _appearance(appearance),
    _palette(k_body_type_options[0]->palette_item().create_palette())
{
}

bn::fixed_point PlayerSprite::position()
{
    if(_body_sprite) 
        return _body_sprite->position();
    return bn::fixed_point();
}

void PlayerSprite::set_position(bn::fixed_point pos)
{
    if(!_body_sprite || !_eyes_sprite || !_top_sprite || !_bottom_sprite || !_hair_sprite)
    {
        return;
    }

    _body_sprite->set_position(pos);
    _eyes_sprite->set_position(pos);
    _top_sprite->set_position(pos);
    _bottom_sprite->set_position(pos);
    _hair_sprite->set_position(pos);
}

void PlayerSprite::set_z_order(int z)
{
    if(!_body_sprite || !_eyes_sprite || !_top_sprite || !_bottom_sprite || !_hair_sprite)
    {
        return;
    }

    _body_sprite->set_z_order(10 * z + 4);
    _eyes_sprite->set_z_order(10 * z + 3);
    _bottom_sprite->set_z_order(10 * z + 2);
    _top_sprite->set_z_order(10 * z + 1);
    _hair_sprite->set_z_order(10 * z);
}

void PlayerSprite::rebuild(const bn::fixed_point& pos)
{
    _rebuild_sprites(pos);
    _sync_sprite(pos);
}

void PlayerSprite::set_scale(int scale)
{
    if(!_body_sprite || !_eyes_sprite || !_top_sprite || !_bottom_sprite || !_hair_sprite)
    {
        return;
    }
    
    _body_sprite->set_scale(scale);
    _eyes_sprite->set_scale(scale);
    _top_sprite->set_scale(scale);
    _bottom_sprite->set_scale(scale);
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

    // Apply color palette
    _appearance.update(_palette);

    // Attach camera if it already exists
    attach_camera();

    // Set z-order (body -> eyes -> bottom -> top -> hair)
    _body_sprite->set_z_order(4);
    _eyes_sprite->set_z_order(3);
    _bottom_sprite->set_z_order(2);
    _top_sprite->set_z_order(1);
    _hair_sprite->set_z_order(0);

    // Set background order
    _body_sprite->set_bg_priority(1);
    _eyes_sprite->set_bg_priority(1);
    _top_sprite->set_bg_priority(1);
    _bottom_sprite->set_bg_priority(1);
    _hair_sprite->set_bg_priority(1);
}

// ---------------------------------------------------------------------------
// Animation update logic
// ---------------------------------------------------------------------------
//
// Per-direction layout for your new sprite sheet (0–27):
//
//  0–3   idle   (4 frames)
//  4–9   walk   (6 frames)
// 10–19  attack (10 frames)
// 20–23  hurt   (4 frames)
// 24–27  death  (4 frames)
//
// There are 28 frames per direction row. You only have rows for
// Down, Right, Up. Left uses the Right row with horizontal flip.
//

void PlayerSprite::_update_movement_animation(bool moving)
{
    constexpr int k_idle_period = 24; // frames between idle flips
    constexpr int k_walk_period = 8;  // frames between walk steps

    _moving = moving;
    ++_anim_counter;

    if(_moving)
    {
        _state = AnimationState::Walk;

        if(_anim_counter >= k_walk_period)
        {
            _anim_counter = 0;
            _walk_frame = (_walk_frame + 1) % 6;   // 0..5 -> frames 4–9
        }
    }
    else
    {
        _state = AnimationState::Idle;

        if(_anim_counter >= k_idle_period)
        {
            _anim_counter = 0;
            _idle_frame = (_idle_frame + 1) % 4;   // 0..3 -> frames 0–3
        }

        // Reset walk cycle when you stop moving
        _walk_frame = 0;
    }
}

void PlayerSprite::_update_attack_animation()
{
    // Attack: 10 frames (10–19)
    constexpr int k_attack_frames = 10;
    constexpr int k_attack_period = 4;  // frames per animation step

    ++_anim_counter;
    if(_anim_counter >= k_attack_period)
    {
        _anim_counter = 0;
        ++_attack_frame;

        if(_attack_frame >= k_attack_frames)
        {
            // End of attack: return to Idle
            _attack_frame = k_attack_frames - 1; // clamp
            _state = AnimationState::Idle;
            _anim_counter = 0;
            _idle_frame = 0;
        }
    }
}

void PlayerSprite::_update_hurt_animation()
{
    // Hurt: 4 frames (20–23)
    constexpr int k_hurt_frames = 4;
    constexpr int k_hurt_period = 6;

    ++_anim_counter;
    if(_anim_counter >= k_hurt_period)
    {
        _anim_counter = 0;
        ++_hurt_frame;

        if(_hurt_frame >= k_hurt_frames)
        {
            // End of hurt: back to Idle
            _hurt_frame = k_hurt_frames - 1;
            _state = AnimationState::Idle;
            _anim_counter = 0;
            _idle_frame = 0;
        }
    }
}

void PlayerSprite::_update_death_animation()
{
    // Death: 4 frames (24–27); stay on last frame
    constexpr int k_death_frames = 4;
    constexpr int k_death_period = 8;

    ++_anim_counter;
    if(_anim_counter >= k_death_period)
    {
        _anim_counter = 0;
        if(_death_frame < k_death_frames - 1)
        {
            ++_death_frame;
        }
        // Once on last frame, remain there with _state = Death
    }
}

void PlayerSprite::_update_block_animation()
{
    // Block loop: 3 frames (28–30)
    constexpr int k_block_frames = 3;
    constexpr int k_block_period = 10;

    ++_anim_counter;
    if(_anim_counter >= k_block_period)
    {
        _anim_counter = 0;
        _block_frame = (_block_frame + 1) % k_block_frames; // 0..2
    }
}

void PlayerSprite::_update_block_success_animation()
{
    // Show the success frame briefly, then revert to Idle.
    // If the player is still holding block, Player::update will call
    // play_block() again and we’ll go back into the normal block loop.
    constexpr int k_block_success_duration = 10; // frames

    ++_block_success_timer;
    if(_block_success_timer >= k_block_success_duration)
    {
        _block_success_timer = 0;
        _state = AnimationState::Idle;
        _anim_counter = 0;
        _idle_frame = 0;
    }
}

void PlayerSprite::_sync_sprite(const bn::fixed_point& pos)
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

    constexpr int k_frames_per_direction = 32;

    // FacingDirection: 0=Down, 1=Right, 2=Up, 3=Left
    int dir = static_cast<int>(_direction);

    // We only have Down, Right, Up rows in the sheet.
    // Left uses the Right row + horizontal flip.
    int sheet_dir = dir;
    bool flip_x = false;

    if(_direction == FacingDirection::Left)
    {
        sheet_dir = static_cast<int>(FacingDirection::Right);
        flip_x = true;
    }

    int base_frame = sheet_dir * k_frames_per_direction;

    int rel_frame = 0;  // 0–27 within that direction row

    switch(_state)
    {
        case AnimationState::Walk:
            // walk: frames 4–9
            rel_frame = 4 + _walk_frame;           // 4 + 0..5
            break;

        case AnimationState::Attack:
            // attack: frames 10–19
            rel_frame = 10 + _attack_frame;        // 10 + 0..9
            break;

        case AnimationState::Hurt:
            // hurt: frames 20–23
            rel_frame = 20 + _hurt_frame;          // 20 + 0..3
            break;

        case AnimationState::Death:
            // death: frames 24–27
            rel_frame = 24 + _death_frame;         // 24 + 0..3
            break;
        
        case AnimationState::Block:
            // block loop: frames 28–30
            rel_frame = 28 + _block_frame; // 28 + 0..2
            break;

        case AnimationState::BlockSuccess:
            // 4th block frame (31)
            rel_frame = 31;
            break;

        case AnimationState::Idle:
        default:
            // idle: frames 0–3
            rel_frame = _idle_frame;               // 0..3
            break;
    }

    int frame_index = base_frame + rel_frame;

    // Apply tiles
    _body_sprite->set_tiles(_body_item->tiles_item(), frame_index);
    _eyes_sprite->set_tiles(_eyes_item->tiles_item(), frame_index);
    _hair_sprite->set_tiles(_hair_item->tiles_item(), frame_index);
    _bottom_sprite->set_tiles(_bottom_item->tiles_item(), frame_index);
    _top_sprite->set_tiles(_top_item->tiles_item(), frame_index);

    // Apply horizontal flip for left-facing
    _body_sprite->set_horizontal_flip(flip_x);
    _eyes_sprite->set_horizontal_flip(flip_x);
    _hair_sprite->set_horizontal_flip(flip_x);
    _bottom_sprite->set_horizontal_flip(flip_x);
    _top_sprite->set_horizontal_flip(flip_x);
}
