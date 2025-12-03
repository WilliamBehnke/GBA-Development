// ---------------------------------------------------------------------------
// character_colors.cpp
// ---------------------------------------------------------------------------

#include "character_colors.h"

namespace
{
    constexpr ColorRamp skin_ramps[k_skin_color_count] = {
        // Pale
        {
            bn::color(24, 20, 18), bn::color(26, 22, 20), bn::color(28, 24, 22), bn::color(30, 26, 24)
        },
        // Tan
        {
            bn::color(18, 12,  8), bn::color(22, 16, 12), bn::color(26, 20, 16), bn::color(30, 24, 20)
        },
        // Dark
        {
            bn::color(10,  6,  4), bn::color(14,  9,  6), bn::color(18, 12,  8), bn::color(22, 16, 12)
        },
    };

    constexpr ColorRamp feature_ramps[k_feature_color_count] = {
        // Red
        {
            bn::color(12,  2,  2), bn::color(18,  4,  4), bn::color(24,  8,  8), bn::color(30, 12, 12)
        },
        // Green
        {
            bn::color( 2, 10,  2), bn::color( 4, 16,  4), bn::color( 8, 22,  8), bn::color(12, 28, 12)
        },
        // Yellow
        {
            bn::color(12, 12,  2), bn::color(18, 18,  4), bn::color(24, 24,  8), bn::color(30, 30, 12)
        },
        // Brown
        {
            bn::color( 8,  4,  2), bn::color(12,  8,  4), bn::color(18, 12,  6), bn::color(24, 18, 10)
        },
        // Blue
        {
            bn::color( 2,  4, 12), bn::color( 4,  8, 18), bn::color( 8, 12, 24), bn::color(12, 18, 30)
        },
        // Purple
        {
            bn::color(10,  2, 12), bn::color(14,  4, 18), bn::color(20,  8, 26), bn::color(28, 12, 30)
        },
        // Cyan
        {
            bn::color( 2, 12, 14), bn::color( 4, 18, 22), bn::color( 8, 24, 30), bn::color(12, 30, 31)
        },
    };
}

const ColorRamp& get_skin_ramp(BodyColor color)
{
    return skin_ramps[static_cast<int>(color)];
}

const ColorRamp& get_feature_ramp(FeatureColor color)
{
    return feature_ramps[static_cast<int>(color)];
}

void ColorRamp::apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const
{
    pal.set_color(1, c0);
    pal.set_color(2, c1);
    pal.set_color(3, c2);
    pal.set_color(4, c3);
}
