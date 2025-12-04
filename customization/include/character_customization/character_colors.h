#ifndef CHARACTER_COLORS_H
#define CHARACTER_COLORS_H

// ---------------------------------------------------------------------------
// character_colors.h
// Predefined color ramps for skin and other features.
// ---------------------------------------------------------------------------

#include "bn_color.h"
#include "bn_sprite_palette_ptr.h"

// Only body skin tones
enum class BodyColor : int
{
    Pale = 0,
    Tan  = 1,
    Dark = 2,
    Count
};

// Shared across hair, eyes, clothing, etc.
enum class FeatureColor : int
{
    Red    = 0,
    Green  = 1,
    Yellow = 2,
    Brown  = 3,
    Blue   = 4,
    Purple = 5,
    Cyan   = 6,
    Count
};

struct ColorRamp
{
    // 4 shades from darkest to lightest
    bn::color c0;
    bn::color c1;
    bn::color c2;
    bn::color c3;

    void apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const;
};

constexpr int k_skin_color_count    = static_cast<int>(BodyColor::Count);
constexpr int k_feature_color_count = static_cast<int>(FeatureColor::Count);

const ColorRamp& get_skin_ramp(BodyColor color);
const ColorRamp& get_feature_ramp(FeatureColor color);

#endif // CHARACTER_COLORS_H
