#ifndef CHARACTER_SPRITE_H
#define CHARACTER_SPRITE_H

#include "bn_fixed_point.h"

// Shared facing directions for all character-like sprites
enum class direction
{
    DOWN,
    RIGHT,
    LEFT,
    UP
};

// General sprite interface for controllable / animated characters.
// The Player uses this for the playable character,
// and enemies can also implement it to reuse animation logic.
class CharacterSprite
{
public:
    virtual ~CharacterSprite() = default;

    // Position ---------------------------------------------------------
    virtual bn::fixed_point position() const = 0;
    virtual void set_position(const bn::fixed_point& pos) = 0;

    // Animation state --------------------------------------------------
    // For player-controlled sprites, these are driven by input.
    // For enemies, they can be driven by AI or be simple no-ops if unused.
    virtual void set_idle(direction dir) = 0;
    virtual void set_walk(direction dir) = 0;
    virtual void start_hurt(direction dir) = 0;

    // Start an attack in a given direction.
    virtual void start_attack(direction dir) = 0;

    // True while an attack animation is playing.
    virtual bool is_attacking() const = 0;

    // True while hurt animation is playing.
    virtual bool is_hurt() const = 0;

    // Per-frame update: advance animations / internal behavior.
    virtual void update() = 0;
};

#endif // CHARACTER_SPRITE_H
