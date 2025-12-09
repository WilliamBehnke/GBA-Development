#include "hitbox.h"
#include "bn_math.h"

Hitbox::Hitbox(bn::fixed ox, bn::fixed oy, bn::fixed hw, bn::fixed hh) :
    offset_x(ox),
    offset_y(oy),
    half_width(hw),
    half_height(hh)
{}

bn::fixed_point Hitbox::center(const bn::fixed_point& sprite_pos) const {
    return bn::fixed_point(
        sprite_pos.x() + offset_x,
        sprite_pos.y() + offset_y
    );
}

bool hitboxes_intersect(
    const Hitbox& a, const bn::fixed_point& pos_a,
    const Hitbox& b, const bn::fixed_point& pos_b
) {
    const bn::fixed_point ca = a.center(pos_a);
    const bn::fixed_point cb = b.center(pos_b);

    const bn::fixed dx = ca.x() - cb.x();
    const bn::fixed dy = ca.y() - cb.y();

    const bn::fixed sum_hw = a.half_width  + b.half_width;
    const bn::fixed sum_hh = a.half_height + b.half_height;

    return (bn::abs(dx) <= sum_hw) && (bn::abs(dy) <= sum_hh);
}
