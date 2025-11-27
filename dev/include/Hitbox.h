#ifndef HITBOX_H
#define HITBOX_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_math.h"

struct Hitbox
{
    bn::fixed offset_x = 0;
    bn::fixed offset_y = 0;
    bn::fixed half_width = 0;
    bn::fixed half_height = 0;

    Hitbox() = default;

    Hitbox(bn::fixed ox, bn::fixed oy, bn::fixed hw, bn::fixed hh) :
        offset_x(ox),
        offset_y(oy),
        half_width(hw),
        half_height(hh)
    {
    }

    bn::fixed_point center(const bn::fixed_point& sprite_pos) const
    {
        return bn::fixed_point(
            sprite_pos.x() + offset_x,
            sprite_pos.y() + offset_y
        );
    }
};

// Simple AABB intersection
inline bool hitboxes_intersect(const Hitbox& a, const bn::fixed_point& pos_a,
                               const Hitbox& b, const bn::fixed_point& pos_b)
{
    bn::fixed_point ca = a.center(pos_a);
    bn::fixed_point cb = b.center(pos_b);

    bn::fixed dx = ca.x() - cb.x();
    bn::fixed dy = ca.y() - cb.y();

    bn::fixed sum_hw = a.half_width + b.half_width;
    bn::fixed sum_hh = a.half_height + b.half_height;

    return (bn::abs(dx) <= sum_hw) && (bn::abs(dy) <= sum_hh);
}

#endif // HITBOX_H
