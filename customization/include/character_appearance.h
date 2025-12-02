#ifndef CHARACTER_APPEARANCE_H
#define CHARACTER_APPEARANCE_H

// ---------------------------------------------------------------------------
// character_appearance.h
// Data model for the current character look.
// ---------------------------------------------------------------------------

enum class FacingDirection : int
{
    Down  = 0,
    Right = 1,
    Left  = 2,
    Up    = 3
};

struct CharacterAppearance
{
    // Which sprite option is used for each component:
    int body_type_index = 0;  // was skin_index: which base/body option
    int hair_index      = 0;
    int eyes_index      = 0;  // we keep this but don't let the user change it
    int top_index       = 0;
    int bottom_index    = 0;

    // Which color palette is used for each component:
    // Body:   0 = pale, 1 = tan, 2 = dark
    // Others: 0 = red, 1 = green, 2 = yellow, 3 = brown, 4 = blue
    int body_color_index   = 2;  // [0..2]
    int hair_color_index   = 3;  // [0..4]
    int eyes_color_index   = 4;  // [0..4]
    int top_color_index    = 1;  // [0..4]
    int bottom_color_index = 2;  // [0..4]

    FacingDirection direction = FacingDirection::Down;
};

#endif // CHARACTER_APPEARANCE_H
