#include "GokuSprite.h"
#include "bn_log.h"

// ---------------------------------------------------------------------
// GOKU SPRITE SHEET LAYOUT (tile indices)
// ---------------------------------------------------------------------
// The sprite sheet is organized in three major sections:
//   1. IDLE frames
//   2. Walk cycles
//   3. Attack animations (multi-combo, multi-direction)
//
// ---------------------------------------------------------------------
// 1. IDLE ANIMATIONS (indices 0–7)
// Each direction has 2 idle frames: main + blink
//
//   DOWN  : main=0, blink=1
//   RIGHT : main=2, blink=3
//   LEFT  : main=4, blink=5
//   UP    : main=6, blink=6   // UP has no blink; reuse main frame
//
// Frame formula:
//   main  = direction * 2
//   blink = main + 1   (except UP, where blink = main)
// ---------------------------------------------------------------------
//
// 2. WALK CYCLES (indices 7–22)
// Each direction uses 4 consecutive frames.
//
//   DOWN  :  7–10
//   RIGHT : 11–14
//   LEFT  : 15–18
//   UP    : 19–22
//
// Frame formula:
//   base = 7 + (direction * 4)
//   walk frames = base, base+1, base+2, base+3
// ---------------------------------------------------------------------
//
// 3. ATTACK ANIMATIONS (indices 23+)
// Attack frames are grouped by direction, then by combo.
// Layout:
//     - 4 directions: DOWN, RIGHT, LEFT, UP
//     - 3 combos per direction: RIGHT_HOOK, LEFT_HOOK, KICK
//     - 4 frames per combo
//
// The DOWN attacks begin at index 23. Blocks follow this pattern:
//
//   block_index = direction_index * 3 + combo_index
//   base        = 23 + block_index * 4
//
// Example:
//   direction = RIGHT (1), combo = LEFT_HOOK (1)
//   block_index = 1*3 + 1 = 4
//   base = 23 + 4*4 = 39
//
// Attack frames = base, base+1, base+2, base+3
// ---------------------------------------------------------------------

// Public API -----------------------------------------------------------

GokuSprite::GokuSprite() :
    _sprite(bn::sprite_items::goku.create_sprite(0, 0)),
    _tiles(bn::sprite_items::goku.tiles_item()),
    _state(State::IDLE),
    _dir(direction::DOWN),
    _combo(AttackCombo::RIGHT_HOOK)
{
    _idle_anim = _make_idle_anim(_dir);
}

bn::fixed_point GokuSprite::position() const
{
    return _sprite.position();
}

void GokuSprite::set_position(const bn::fixed_point& pos)
{
    _sprite.set_position(pos);
}

void GokuSprite::set_idle(direction dir)
{
    if(_state == State::IDLE && _dir == dir)
    {
        return; // already idle facing that direction
    }

    _state = State::IDLE;
    _dir = dir;
    _walk_anim.reset();
    _attack_anim.reset();
    _hurt_anim.reset();
    _idle_anim = _make_idle_anim(_dir);
}

void GokuSprite::set_walk(direction dir)
{
    if(_state == State::WALK && _dir == dir)
    {
        return; // already walking this way
    }

    _state = State::WALK;
    _dir = dir;
    _idle_anim.reset();
    _attack_anim.reset();
    _hurt_anim.reset();
    _walk_anim = _make_walk_anim(_dir);
}

void GokuSprite::start_hurt(direction dir)
{
    BN_LOG("Hurting!");
    _state = State::HURT;
    _dir = dir;
    _idle_anim.reset();
    _walk_anim.reset();
    _attack_anim.reset();
    _hurt_anim = _make_hurt_anim(_dir);
}

void GokuSprite::start_attack(direction dir)
{
    BN_LOG("Attacking!");
    _state = State::ATTACK;
    _dir = dir;
    _idle_anim.reset();
    _walk_anim.reset();
    _hurt_anim.reset();

    _attack_anim = _make_attack_anim(_dir, _combo);

    // Advance combo for the next attack
    _combo = _next_combo(_combo);
}

bool GokuSprite::is_attacking() const
{
    return _state == State::ATTACK && _attack_anim.has_value();
}

bool GokuSprite::is_hurt() const
{
    return _state == State::HURT && _hurt_anim.has_value();
}

void GokuSprite::update()
{
    switch(_state)
    {
    case State::ATTACK:
        if(_attack_anim.has_value())
        {
            _attack_anim->update();

            if(_attack_anim->done())
            {
                _attack_anim.reset();
                // After attack, fall back to idle in same direction
                set_idle(_dir);
            }
        }
        break;

    case State::WALK:
        if(_walk_anim.has_value())
        {
            _walk_anim->update();
        }
        break;

    case State::HURT:
        if(_hurt_anim.has_value())
        {
            _hurt_anim->update();

            if(_hurt_anim->done())
            {
                _hurt_anim.reset();
                set_idle(_dir);
            }
        }
        break;

    case State::IDLE:
        if(_idle_anim.has_value())
        {
            _idle_anim->update();
        }
        break;
    default:
        break;
    }
}

// Private helpers ------------------------------------------------------

GokuSprite::AttackCombo GokuSprite::_next_combo(AttackCombo c)
{
    int i = (static_cast<int>(c) + 1) % 3;
    return static_cast<AttackCombo>(i);
}

bn::sprite_animate_action<4> GokuSprite::_make_attack_anim(direction facing, AttackCombo combo)
{
    constexpr int combos_per_direction = 3;
    constexpr int frames_per_combo = 4;
    constexpr int down_attack_start = 23;

    int dir_index = static_cast<int>(facing);
    int combo_index = static_cast<int>(combo);
    int base = down_attack_start + (dir_index * combos_per_direction + combo_index) * frames_per_combo;

    return bn::create_sprite_animate_action_once(
        _sprite,
        6,
        _tiles,
        base, base + 1, base + 2, base + 3
    );
}

bn::sprite_animate_action<4> GokuSprite::_make_walk_anim(direction facing)
{
    constexpr int frames_per_walk = 4;
    constexpr int down_walk_start = 7;

    int dir_index = static_cast<int>(facing);
    int base = down_walk_start + dir_index * frames_per_walk;

    return bn::create_sprite_animate_action_forever(
        _sprite,
        6,
        _tiles,
        base, base + 1, base + 2, base + 3
    );
}

bn::sprite_animate_action<10> GokuSprite::_make_idle_anim(direction facing)
{
    int main_frame = static_cast<int>(facing) * 2;
    int blink_frame = main_frame + 1;

    // For UP, we can just hold the same frame (no real "blink")
    if(facing == direction::UP)
    {
        blink_frame = main_frame;
    }

    return bn::create_sprite_animate_action_forever(
        _sprite,
        12,
        _tiles,
        main_frame, main_frame, main_frame, main_frame, main_frame,
        main_frame, main_frame, main_frame, main_frame, blink_frame
    );
}

bn::sprite_animate_action<4> GokuSprite::_make_hurt_anim(direction facing)
{
    constexpr int frames_per_dir = 4;
    constexpr int down_start = 71;

    int dir_index = static_cast<int>(facing);
    int base = down_start + dir_index * frames_per_dir;

    return bn::create_sprite_animate_action_once(
        _sprite,
        6,
        _tiles,
        base, base + 1, base + 2, base + 3
    );
}
