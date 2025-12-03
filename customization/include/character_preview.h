#ifndef CHARACTER_PREVIEW_H
#define CHARACTER_PREVIEW_H

// ---------------------------------------------------------------------------
// character_preview.h
// Draws and animates the layered character in the customization screen,
// and applies palette-based color ramps.
// ---------------------------------------------------------------------------

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "character_colors.h"
#include "character_appearance.h"

// Simple layered preview: body, eyes, top, bottom, hair
class CharacterPreview
{
public:
    explicit CharacterPreview(const bn::fixed_point& pos);

    void set_direction(FacingDirection dir);
    void set_layers(const bn::sprite_item* body,
                    const bn::sprite_item* eyes,
                    const bn::sprite_item* top,
                    const bn::sprite_item* bottom,
                    const bn::sprite_item* hair);

    void apply_colors(const CharacterAppearance& appearance);

    void set_scale(int scale);
    void set_position(const bn::fixed_point& pos);

    void update();

    const bn::fixed_point& position() const { return _pos; }

private:
    bn::fixed_point _pos;
    FacingDirection _direction = FacingDirection::Down;

    // Layered sprites
    bn::optional<bn::sprite_ptr> _body_sprite;
    bn::optional<bn::sprite_ptr> _eyes_sprite;
    bn::optional<bn::sprite_ptr> _top_sprite;
    bn::optional<bn::sprite_ptr> _bottom_sprite;
    bn::optional<bn::sprite_ptr> _hair_sprite;

    // Sprite items (so we can set tiles each frame)
    const bn::sprite_item* _body_item   = nullptr;
    const bn::sprite_item* _eyes_item   = nullptr;
    const bn::sprite_item* _top_item    = nullptr;
    const bn::sprite_item* _bottom_item = nullptr;
    const bn::sprite_item* _hair_item   = nullptr;

    // Idle animation (2 frames per direction)
    int _idle_counter = 0;
    int _idle_frame   = 0;    // 0 or 1

    void _rebuild_sprites();
    void _sync_sprites();     // apply pos + frame index
};

#endif // CHARACTER_PREVIEW_H
