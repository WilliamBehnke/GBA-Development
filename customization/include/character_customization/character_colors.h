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
    Pale  = 0,
    Tan   = 1,
    Dark  = 2,
    Count
};

// Shared across hair, eyes, clothing, etc.
enum class FeatureColor : int
{
    Red      = 0,
    Blue     = 1,
    Green    = 2,
    Yellow   = 3,
    Lavender = 4,
    Caramel  = 5,
    Brown    = 6,
    Black    = 7,
    Count
};

class ColorRamp
{
public:
    virtual ~ColorRamp() = default;

    // Apply this ramp to a sprite palette
    virtual void apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const = 0;
};

class SkinColorRamp : public ColorRamp
{
public:
    // 5 shades from darkest to lightest
    bn::color c0;
    bn::color c1;
    bn::color c2;
    bn::color c3;
    bn::color c4;

    SkinColorRamp(
        bn::color c0_, bn::color c1_, bn::color c2_, bn::color c3_, 
        bn::color c4_) :
        c0(c0_), c1(c1_), c2(c2_), c3(c3_), c4(c4_)
    {}

    void apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const override;
};

class FeatureColorRamp : public ColorRamp
{
public:
    // 7 shades from darkest to lightest
    bn::color c0;
    bn::color c1;
    bn::color c2;
    bn::color c3;
    bn::color c4;
    bn::color c5;
    bn::color c6;

    FeatureColorRamp(
        bn::color c0_, bn::color c1_, bn::color c2_, bn::color c3_,
        bn::color c4_, bn::color c5_, bn::color c6_) :
        c0(c0_), c1(c1_), c2(c2_), c3(c3_), c4(c4_), c5(c5_), c6(c6_)
    {}

    void apply_ramp_to_palette(bn::sprite_palette_ptr& pal) const override;
};

constexpr int k_skin_color_count    = static_cast<int>(BodyColor::Count);
constexpr int k_feature_color_count = static_cast<int>(FeatureColor::Count);

const SkinColorRamp& get_skin_ramp(BodyColor color);
const FeatureColorRamp& get_feature_ramp(FeatureColor color);

void update_palette(
    bn::sprite_palette_ptr& pal,
    BodyColor    body_color, 
    FeatureColor eyes_color, 
    FeatureColor hair_color, 
    FeatureColor top_color, 
    FeatureColor bottom_color
);

#endif // CHARACTER_COLORS_H
