#include "SnakeSprite.h"

#include "bn_fixed.h"

// ---------------------------------------------------------------------
// SNAKE SPRITE SHEET LAYOUT (tile indices)
// ---------------------------------------------------------------------
// For each animation type we group frames by direction in this order:
//   UP, RIGHT, LEFT, DOWN  -> direction_index = 0, 1, 2, 3
//
// Per-direction frame counts:
//   Idle   : 1 frame
//   Walk   : 4 frames
//   Attack : 4 frames
//   Hurt   : 3 frames
//
// Sequence:
//
//   [Idle  (1 frame * 4 directions)]  ->  4 tiles total (0–3)
//   [Walk  (4 frames * 4 directions)] -> 16 tiles total
//   [Attack(4 frames * 4 directions)] -> 16 tiles total
//   [Hurt  (3 frames * 4 directions)] -> 12 tiles total
//
// Ranges:
//
//   Idle:
//     IDLE_START = 0
//     frame = IDLE_START + dir_index
//
//   Walk:
//     WALK_START = IDLE_START + 4 = 4
//     base = WALK_START + dir_index * 4
//     frames = base..base+3
//
//   Attack:
//     ATTACK_START = WALK_START + 16 = 20
//     base = ATTACK_START + dir_index * 4
//     frames = base..base+3
//
//   Hurt:
//     HURT_START = ATTACK_START + 16 = 36
//     base = HURT_START + dir_index * 3
//     frames = base..base+2
// ---------------------------------------------------------------------

namespace
{
    constexpr bn::fixed k_half_width  = 120;   // 240 / 2
}

SnakeSprite::SnakeSprite(const bn::fixed_point& spawn_pos) :
    _sprite(bn::sprite_items::snake.create_sprite(spawn_pos.x(), spawn_pos.y())),
    _tiles(bn::sprite_items::snake.tiles_item()),
    _state(State::Idle),
    _dir(direction::DOWN),
    _speed(0.5),
    _horizontal_dir(-1)
{
    _idle_anim = _make_idle_anim(_dir);
}

bn::fixed_point SnakeSprite::position() const
{
    return _sprite.position();
}

void SnakeSprite::set_position(const bn::fixed_point& pos)
{
    _sprite.set_position(pos);
}

void SnakeSprite::set_idle(direction dir)
{
    if(_state == State::Idle && _dir == dir)
    {
        return;
    }

    _state = State::Idle;
    _dir = dir;

    _walk_anim.reset();
    _attack_anim.reset();
    _hurt_anim.reset();
    _idle_anim = _make_idle_anim(_dir);
}

void SnakeSprite::set_walk(direction dir)
{
    if(_state == State::Walk && _dir == dir)
    {
        return;
    }

    _state = State::Walk;
    _dir = dir;

    _idle_anim.reset();
    _attack_anim.reset();
    _hurt_anim.reset();
    _walk_anim = _make_walk_anim(_dir);
}

void SnakeSprite::start_hurt(direction dir)
{
    _state = State::Hurt;
    _dir = dir;

    _idle_anim.reset();
    _walk_anim.reset();
    _attack_anim.reset();
    _hurt_anim = _make_hurt_anim(_dir);
}

void SnakeSprite::start_attack(direction dir)
{
    _state = State::Attack;
    _dir = dir;

    _idle_anim.reset();
    _walk_anim.reset();
    _hurt_anim.reset();
    _attack_anim = _make_attack_anim(_dir);
}

bool SnakeSprite::is_attacking() const
{
    return _state == State::Attack && _attack_anim.has_value();
}

bool SnakeSprite::is_hurt() const
{
    return _state == State::Hurt && _hurt_anim.has_value();
}

void SnakeSprite::update()
{
    // Simple horizontal patrol whenever we're not attacking or hurt
    if(_state != State::Attack && _state != State::Hurt)
    {
        bn::fixed_point pos = _sprite.position();
        pos.set_x(pos.x() + _speed * _horizontal_dir);

        if(pos.x() > k_half_width)
        {
            pos.set_x(k_half_width);
            _horizontal_dir = -1;
            set_walk(direction::LEFT);
        }
        else if(pos.x() < -k_half_width)
        {
            pos.set_x(-k_half_width);
            _horizontal_dir = 1;
            set_walk(direction::RIGHT);
        }

        _sprite.set_position(pos);

        if(_state == State::Idle)
        {
            set_walk(_horizontal_dir > 0 ? direction::RIGHT : direction::LEFT);
        }
    }

    switch(_state)
    {
    case State::Attack:
        if(_attack_anim.has_value())
        {
            _attack_anim->update();

            if(_attack_anim->done())
            {
                _attack_anim.reset();
                set_idle(_dir);
            }
        }
        break;

    case State::Walk:
        if(_walk_anim.has_value())
        {
            _walk_anim->update();
        }
        break;

    case State::Hurt:
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

    case State::Idle:
    default:
        if(_idle_anim.has_value())
        {
            _idle_anim->update();
        }
        break;
    }
}

// Internal helpers -----------------------------------------------------

int SnakeSprite::_direction_index(direction dir)
{
    // Sprite sheet order: UP, RIGHT, LEFT, DOWN  -> 0, 1, 2, 3
    switch(dir)
    {
    case direction::UP:    return 0;
    case direction::RIGHT: return 1;
    case direction::LEFT:  return 2;
    case direction::DOWN:  return 3;
    default:               return 3;
    }
}

bn::sprite_animate_action<2> SnakeSprite::_make_idle_anim(direction facing)
{
    constexpr int IDLE_START = 0;

    int dir_index = _direction_index(facing);
    int frame = IDLE_START + dir_index;

    // Butano requires MaxSize > 1, so we repeat the same tile twice.
    return bn::create_sprite_animate_action_forever(
        _sprite,
        15,         // delay
        _tiles,
        frame, frame
    );
}

bn::sprite_animate_action<4> SnakeSprite::_make_walk_anim(direction facing)
{
    constexpr int IDLE_COUNT   = 4;   // 1 * 4
    constexpr int WALK_START   = IDLE_COUNT;

    int dir_index = _direction_index(facing);
    int base = WALK_START + dir_index * 4;

    return bn::create_sprite_animate_action_forever(
        _sprite,
        6,
        _tiles,
        base, base + 1, base + 2, base + 3
    );
}

bn::sprite_animate_action<4> SnakeSprite::_make_attack_anim(direction facing)
{
    constexpr int IDLE_COUNT   = 4;
    constexpr int WALK_COUNT   = 16;
    constexpr int ATTACK_START = IDLE_COUNT + WALK_COUNT;   // 20

    int dir_index = _direction_index(facing);
    int base = ATTACK_START + dir_index * 4;

    return bn::create_sprite_animate_action_once(
        _sprite,
        5,
        _tiles,
        base, base + 1, base + 2, base + 3
    );
}

bn::sprite_animate_action<3> SnakeSprite::_make_hurt_anim(direction facing)
{
    constexpr int IDLE_COUNT   = 4;
    constexpr int WALK_COUNT   = 16;
    constexpr int ATTACK_COUNT = 16;
    constexpr int HURT_START   = IDLE_COUNT + WALK_COUNT + ATTACK_COUNT;   // 36

    int dir_index = _direction_index(facing);
    int base = HURT_START + dir_index * 3;

    return bn::create_sprite_animate_action_once(
        _sprite,
        5,
        _tiles,
        base, base + 1, base + 2
    );
}
