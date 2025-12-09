#ifndef CHARACTER_APPEARANCE_H
#define CHARACTER_APPEARANCE_H

// ---------------------------------------------------------------------------
// character_appearance.h
// Data model for the current character look.
// ---------------------------------------------------------------------------

#include "character_colors.h"

enum class FacingDirection : int
{
    Down  = 0,
    Right = 1,
    Up    = 2,
    Left  = 3
};

struct CharacterAppearance
{
    // Which sprite option is used for each component
    int hair_index      = 1;
    int top_index       = 0;
    int bottom_index    = 0;

    // Which color palette is used for each component
    BodyColor body_color        = BodyColor::Tan;
    FeatureColor hair_color     = FeatureColor::Brown;
    FeatureColor eyes_color     = FeatureColor::Red;
    FeatureColor top_color      = FeatureColor::Green;
    FeatureColor bottom_color   = FeatureColor::Blue;

    FacingDirection direction = FacingDirection::Down;

    void update(bn::sprite_palette_ptr& pal) const
    {
        update_palette(
            pal,
            body_color, 
            eyes_color, 
            hair_color, 
            top_color, 
            bottom_color
        );
    }
};

#endif // CHARACTER_APPEARANCE_H
