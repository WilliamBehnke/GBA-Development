#ifndef CHARACTER_MODEL_H
#define CHARACTER_MODEL_H

// ---------------------------------------------------------------------------
// character_model.h
// Tracks whether style or colors changed since last "apply to view".
// ---------------------------------------------------------------------------

#include "character_appearance.h"

class CharacterModel
{
public:
    CharacterModel()
    {
        _style_dirty  = true;
        _colors_dirty = true;
    }

    const CharacterAppearance& appearance() const
    {
        return _appearance;
    }

    CharacterAppearance& appearance()
    {
        return _appearance;
    }

    void mark_style_dirty()
    {
        _style_dirty = true;
    }

    void mark_colors_dirty()
    {
        _colors_dirty = true;
    }

    bool style_dirty() const
    {
        return _style_dirty;
    }

    bool colors_dirty() const
    {
        return _colors_dirty;
    }

    void clear_dirty()
    {
        _style_dirty  = false;
        _colors_dirty = false;
    }

private:
    CharacterAppearance _appearance;
    bool _style_dirty  = false;
    bool _colors_dirty = false;
};

#endif // CHARACTER_MODEL_H
