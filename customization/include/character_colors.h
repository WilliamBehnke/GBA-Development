#ifndef CHARACTER_COLORS_H
#define CHARACTER_COLORS_H

// ---------------------------------------------------------------------------
// character_colors.h
// Predefined color ramps for skin and other features.
// ---------------------------------------------------------------------------

#include "bn_color.h"

struct ColorRamp
{
    // 4 shades from darkest to lightest
    bn::color c0;
    bn::color c1;
    bn::color c2;
    bn::color c3;
};

// Counts:
constexpr int k_skin_color_count    = 3;  // pale, tan, dark
constexpr int k_feature_color_count = 5;  // red, green, yellow, brown, blue

// Get ramps by index; indices are automatically wrapped/clamped.
const ColorRamp& get_skin_ramp(int index);
const ColorRamp& get_feature_ramp(int index);

#endif // CHARACTER_COLORS_H
