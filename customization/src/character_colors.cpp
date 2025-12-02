// ---------------------------------------------------------------------------
// character_colors.cpp
// ---------------------------------------------------------------------------

#include "character_colors.h"

namespace
{
    // Skin ramps: pale, tan, dark
    constexpr ColorRamp skin_ramps[k_skin_color_count] = {
        // Pale
        {
            bn::color(24, 20, 18),   // dark
            bn::color(26, 22, 20),
            bn::color(28, 24, 22),
            bn::color(30, 26, 24)    // light
        },
        // Tan
        {
            bn::color(18, 12,  8),
            bn::color(22, 16, 12),
            bn::color(26, 20, 16),
            bn::color(30, 24, 20)
        },
        // Dark
        {
            bn::color(10,  6,  4),
            bn::color(14,  9,  6),
            bn::color(18, 12,  8),
            bn::color(22, 16, 12)
        },
    };

    // Feature ramps (hair, eyes, top, bottom):
    // red, green, yellow, brown, blue
    constexpr ColorRamp feature_ramps[k_feature_color_count] = {
        // Red
        {
            bn::color(12,  2,  2),
            bn::color(18,  4,  4),
            bn::color(24,  8,  8),
            bn::color(30, 12, 12)
        },
        // Green
        {
            bn::color( 2, 10,  2),
            bn::color( 4, 16,  4),
            bn::color( 8, 22,  8),
            bn::color(12, 28, 12)
        },
        // Yellow
        {
            bn::color(12, 12,  2),
            bn::color(18, 18,  4),
            bn::color(24, 24,  8),
            bn::color(30, 30, 12)
        },
        // Brown
        {
            bn::color( 8,  4,  2),
            bn::color(12,  8,  4),
            bn::color(18, 12,  6),
            bn::color(24, 18, 10)
        },
        // Blue
        {
            bn::color( 2,  4, 12),
            bn::color( 4,  8, 18),
            bn::color( 8, 12, 24),
            bn::color(12, 18, 30)
        },
    };

    int wrap_index(int index, int count)
    {
        if(count <= 0)
        {
            return 0;
        }

        if(index < 0)
        {
            index += count * ((-index / count) + 1);
        }

        index %= count;
        return index;
    }
}

// ---------------------------------------------------------------------------

const ColorRamp& get_skin_ramp(int index)
{
    int idx = wrap_index(index, k_skin_color_count);
    return skin_ramps[idx];
}

const ColorRamp& get_feature_ramp(int index)
{
    int idx = wrap_index(index, k_feature_color_count);
    return feature_ramps[idx];
}
