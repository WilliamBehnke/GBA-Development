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
    int hair_index      = 0;
    int top_index       = 0;
    int bottom_index    = 0;

    // Which color palette is used for each component
    BodyColor body_color        = BodyColor::Tan;
    FeatureColor hair_color     = FeatureColor::Brown;
    FeatureColor eyes_color     = FeatureColor::Blue;
    FeatureColor top_color      = FeatureColor::Green;
    FeatureColor bottom_color   = FeatureColor::Yellow;

    FacingDirection direction = FacingDirection::Down;
};

#endif // CHARACTER_APPEARANCE_H
