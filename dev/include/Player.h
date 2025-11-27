#ifndef PLAYER_H
#define PLAYER_H

#include "CharacterSprite.h"

// Player: handles input, movement, and high-level state,
// but delegates all visual / animation details to CharacterSprite.
class Player
{
public:
    explicit Player(CharacterSprite& sprite);

    // Called once per frame
    void update();

private:
    CharacterSprite& _sprite;
    direction _facing;
};

#endif // PLAYER_H
