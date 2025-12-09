#ifndef HITBOX_H
#define HITBOX_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

struct Hitbox
{
    bn::fixed offset_x = 0;
    bn::fixed offset_y = 0;
    bn::fixed half_width = 0;
    bn::fixed half_height = 0;

    Hitbox() = default;

    Hitbox(bn::fixed ox, bn::fixed oy, bn::fixed hw, bn::fixed hh);

    // Returns world-space center of the hitbox
    bn::fixed_point center(const bn::fixed_point& sprite_pos) const;
};

// Simple AABB intersection test
bool hitboxes_intersect(
    const Hitbox& a, const bn::fixed_point& pos_a,
    const Hitbox& b, const bn::fixed_point& pos_b
);

#endif // HITBOX_H
