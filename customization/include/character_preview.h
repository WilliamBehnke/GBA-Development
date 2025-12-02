#ifndef CHARACTER_PREVIEW_H
#define CHARACTER_PREVIEW_H

// ---------------------------------------------------------------------------
// character_preview.h
// Draws and animates the layered character in the customization screen,
// and applies palette-based color ramps.
// ---------------------------------------------------------------------------

#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"

#include "character_appearance.h"

namespace bn
{
    class sprite_item;
}

class CharacterPreview
{
public:
    explicit CharacterPreview(const bn::fixed_point& position);

    // Call every frame to advance idle animations
    void update();

    // Set which sprite items to use for each layer
    // Any pointer can be nullptr to hide that layer.
    void set_layers(const bn::sprite_item* body,
                    const bn::sprite_item* eyes,
                    const bn::sprite_item* top,
                    const bn::sprite_item* bottom,
                    const bn::sprite_item* hair);

    // Set the facing direction (rebuilds animations as needed)
    void set_direction(FacingDirection direction);

    void set_scale(int scale);

    // Apply palette colors to all components, based on appearance
    void apply_colors(const CharacterAppearance& appearance);

private:
    void _rebuild_layers();

    bn::fixed_point _pos;
    FacingDirection _direction = FacingDirection::Down;

    // Currently selected sprite items
    const bn::sprite_item* _body_item   = nullptr;  // was _skin_item
    const bn::sprite_item* _eyes_item   = nullptr;
    const bn::sprite_item* _top_item    = nullptr;
    const bn::sprite_item* _bottom_item = nullptr;
    const bn::sprite_item* _hair_item   = nullptr;

    // Layer sprites
    bn::optional<bn::sprite_ptr> _body_sprite;
    bn::optional<bn::sprite_ptr> _eyes_sprite;
    bn::optional<bn::sprite_ptr> _top_sprite;
    bn::optional<bn::sprite_ptr> _bottom_sprite;
    bn::optional<bn::sprite_ptr> _hair_sprite;

    // Idle animation (2 frames) per layer
    bn::optional<bn::sprite_animate_action<2>> _body_anim;
    bn::optional<bn::sprite_animate_action<2>> _eyes_anim;
    bn::optional<bn::sprite_animate_action<2>> _top_anim;
    bn::optional<bn::sprite_animate_action<2>> _bottom_anim;
    bn::optional<bn::sprite_animate_action<2>> _hair_anim;
};

#endif // CHARACTER_PREVIEW_H
