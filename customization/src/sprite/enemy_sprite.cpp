// ---------------------------------------------------------------------------
// enemy_sprite.cpp
// ---------------------------------------------------------------------------

#include "enemy_sprite.h"

#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_items_base_0.h"

EnemySprite::EnemySprite(const bn::fixed_point& pos) : _sprite_item(bn::sprite_items::base_0)
{
    _sprite = _sprite_item.create_sprite(pos);
    _sprite->set_bg_priority(1);
}

bn::fixed_point EnemySprite::position()
{
    return _sprite->position();
}

void EnemySprite::set_position(bn::fixed_point pos)
{
    _sprite->set_position(pos);
}

void EnemySprite::set_z_order(int z)
{
    _sprite->set_z_order(10 * z);    // multiply by 10 to allow multi-layered sprites
}

void EnemySprite::attach_camera(const bn::camera_ptr& camera)
{
    _camera = camera;
    if(_sprite)
        _sprite->set_camera(camera);
}

void EnemySprite::detach_camera()
{
    if(!_camera)
        return;

    _camera.reset();
    if(_sprite)
        _sprite->remove_camera();
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

void EnemySprite::_update_movement_animation(bool moving)
{
    constexpr int k_idle_period = 24; // frames between idle flips
    constexpr int k_walk_period = 6;  // frames between walk steps

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

void EnemySprite::_update_attack_animation()
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

void EnemySprite::_update_hurt_animation()
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

void EnemySprite::_update_death_animation()
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
        else
        {
            _sprite->set_z_order(32767);
        }
        // Once on last frame, remain there with _state = Death
    }
}

void EnemySprite::_sync_sprite(const bn::fixed_point& pos)
{
    if(!_sprite)
    {
        return;
    }

    // Position
    _sprite->set_position(pos);

    constexpr int k_frames_per_direction = 28;

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

        case AnimationState::Idle:
        default:
            // idle: frames 0–3
            rel_frame = _idle_frame;               // 0..3
            break;
    }

    int frame_index = base_frame + rel_frame;

    // Apply tiles
    _sprite->set_tiles(_sprite_item.tiles_item(), frame_index);

    // Apply horizontal flip for left-facing
    _sprite->set_horizontal_flip(flip_x);
}
