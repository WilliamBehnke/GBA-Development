#ifndef CHARACTER_PREVIEW_H
#define CHARACTER_PREVIEW_H

// ---------------------------------------------------------------------------
// character_preview.h
// Draws and animates the layered character in the customization screen,
// using PlayerSprite under the hood, and adds a border around it.
// ---------------------------------------------------------------------------

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "character_appearance.h"
#include "player_sprite.h"

// Simple layered preview: wraps PlayerSprite + border
class CharacterPreview
{
public:
    // appearance is the same model used by your customization screen
    CharacterPreview(const bn::fixed_point& pos,
                     const CharacterAppearance& appearance);

    void set_direction(FacingDirection dir);

    // Rebuild sprites / re-apply colors from the current appearance
    void refresh();

    void toggle_animation();

    void set_scale(int scale);
    void set_position(const bn::fixed_point& pos);

    void update();   // idle animation only

    const bn::fixed_point& position() const { return _pos; }

private:
    bn::fixed_point _pos;
    FacingDirection _direction = FacingDirection::Down;

    const CharacterAppearance& _appearance;
    PlayerSprite _sprite;

    bool _moving;

    // Preview border frame
    bn::optional<bn::sprite_ptr> _border;
};

#endif // CHARACTER_PREVIEW_H
