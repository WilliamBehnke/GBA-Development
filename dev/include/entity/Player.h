#ifndef PLAYER_H
#define PLAYER_H

#include "entity/Entity.h"
#include "sprite/BaseSprite.h"

class Player : public Entity {
public:
    Player(BaseSprite* sprite);

    void update();

private:
    direction _facing;
};

#endif // PLAYER_H
