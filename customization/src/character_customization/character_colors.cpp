// ---------------------------------------------------------------------------
// character_colors.cpp
// ---------------------------------------------------------------------------

#include "character_colors.h"
#include "bn_sprite_palette_item.h"

namespace
{
    // Convert 0–255 channel to 0–31 (GBA) with rounding
    constexpr int rgb_to_gba_component(int c_255)
    {
        return (c_255 * 31 + 127) / 255;   // +127 ≈ round instead of floor
    }

    // Build a bn::color from 0–255 RGB values
    constexpr bn::color rgb(int r_255, int g_255, int b_255)
    {
        return bn::color(
            rgb_to_gba_component(r_255),
            rgb_to_gba_component(g_255),
            rgb_to_gba_component(b_255));
    }

    const SkinColorRamp skin_ramps[k_skin_color_count] = {
        // Pale
        SkinColorRamp(
            rgb(122,  64,  45),
            rgb(209, 130, 102),
            rgb(215, 142, 113),
            rgb(229, 168, 134),
            rgb(238, 186, 147)
        ),
        // Tan
        SkinColorRamp(
            rgb(122,  64,  45),
            rgb(180, 111,  88),
            rgb(186, 122,  95),
            rgb(200, 138, 102),
            rgb(214, 153, 110)
        ),
        // Dark
        SkinColorRamp(
            rgb(122,  64,  45),
            rgb(152,  89,  70),
            rgb(159,  99,  76),
            rgb(180, 119,  89),
            rgb(188, 126,  92)
        ),
    };

    const FeatureColorRamp feature_ramps[k_feature_color_count] = {
        // Red
        FeatureColorRamp(
            rgb(170,  34,  54),
            rgb(174,  63,  79),
            rgb(188,  51,  62),
            rgb(190,  80,  89),
            rgb(212, 104, 102),
            rgb(214,  77,  74),
            rgb(253, 164, 183)
        ),
        // Blue
        FeatureColorRamp(
            rgb( 52,  75, 112),
            rgb( 72,  93, 126),
            rgb( 65,  87, 122),
            rgb( 85, 104, 135),
            rgb( 93, 121, 150),
            rgb( 74, 106, 140),
            rgb( 40, 166, 204)
        ),
        // Green
        FeatureColorRamp(
            rgb( 51, 153,  70),
            rgb( 70, 156,  86),
            rgb( 60, 163,  72),
            rgb( 77, 163,  88),
            rgb( 75, 176,  77),
            rgb( 93, 177,  95),
            rgb(124, 240, 100)
        ),
        // Yellow
        FeatureColorRamp(
            rgb(226, 142,  24),
            rgb(220, 155,  64),
            rgb(228, 157,  32),
            rgb(223, 167,  71),
            rgb(228, 189,  82),
            rgb(234, 184,  46),
            rgb(245, 238,  86)
        ),
        // Lavender
        FeatureColorRamp(
            rgb(133, 101, 163),
            rgb(142, 111, 170),
            rgb(146, 120, 172),
            rgb(154, 129, 178),
            rgb(159, 140, 191),
            rgb(169, 154, 196),
            rgb(233, 184, 255)
        ),
        // Caramel
        FeatureColorRamp(
            rgb(201,  96,  22),
            rgb(199, 116,  58),
            rgb(211, 114,  23),
            rgb(208, 132,  61),
            rgb(220, 155,  64),
            rgb(226, 142,  24),
            rgb(248, 204,  76)
        ),
        // Brown
        FeatureColorRamp(
            rgb(124,  62,  43),
            rgb(136,  69,  51),
            rgb(136,  82,  65),
            rgb(146,  89,  73),
            rgb(148,  84,  63),
            rgb(156, 102,  85),
            rgb(230, 133,  97)
        ),
        // Black
        FeatureColorRamp(
            rgb( 66,  30,  45),
            rgb( 86,  48,  64),
            rgb( 76,  39,  52),
            rgb( 95,  58,  71),
            rgb(108,  71,  80),
            rgb( 91,  52,  62),
            rgb(204, 102,  110)
        ),
    };
}

const SkinColorRamp& get_skin_ramp(BodyColor color)
{
    return skin_ramps[static_cast<int>(color)];
}

const FeatureColorRamp& get_feature_ramp(FeatureColor color)
{
    return feature_ramps[static_cast<int>(color)];
}

void SkinColorRamp::apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const
{
    pal.set_color(1, c0);
    pal.set_color(2, c1);
    pal.set_color(3, c2);
    pal.set_color(4, c3);
    pal.set_color(5, c4);
}

void FeatureColorRamp::apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const
{
    pal.set_color(1, c0);
    pal.set_color(2, c1);
    pal.set_color(3, c2);
    pal.set_color(4, c3);
    pal.set_color(5, c4);
    pal.set_color(6, c5);
    pal.set_color(7, c6);
}

// ---------------------------------------------------------------------------
// Character palette layout (8bpp)
//
//  0        transparent
//  1-4      skin color
//  5-6      eye color
//  7-10     hair color
// 11-12     top color
// 13-15     pants color
// ---------------------------------------------------------------------------

void update_palette(
    bn::sprite_palette_ptr& pal,
    BodyColor    body_color, 
    FeatureColor eyes_color, 
    FeatureColor hair_color, 
    FeatureColor top_color, 
    FeatureColor bottom_color)
{
    // Get ramps
    const SkinColorRamp&    skin_ramp   = get_skin_ramp(body_color);
    const FeatureColorRamp& eye_ramp    = get_feature_ramp(eyes_color);
    const FeatureColorRamp& hair_ramp   = get_feature_ramp(hair_color);
    const FeatureColorRamp& top_ramp    = get_feature_ramp(top_color);
    const FeatureColorRamp& bottom_ramp = get_feature_ramp(bottom_color);

    // 1-4: skin color
    pal.set_color(1, skin_ramp.c0);
    pal.set_color(2, skin_ramp.c1);
    pal.set_color(3, skin_ramp.c2);
    pal.set_color(4, skin_ramp.c3);

    // 5-6: eye color
    pal.set_color(5, eye_ramp.c1);
    pal.set_color(6, eye_ramp.c4);

    // 7-10: hair color
    pal.set_color(7, hair_ramp.c0);
    pal.set_color(8, hair_ramp.c2);
    pal.set_color(9, hair_ramp.c5);
    pal.set_color(10, hair_ramp.c6);

    // 11-12: top color
    pal.set_color(11, top_ramp.c2);
    pal.set_color(12, top_ramp.c5);

    // 13-15: bottom color
    pal.set_color(13, bottom_ramp.c1);
    pal.set_color(14, bottom_ramp.c3);
    pal.set_color(15, bottom_ramp.c4);
}
