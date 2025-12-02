#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

#include "character_preview.h"   // for CharacterAppearance + FacingDirection
#include "character_colors.h"

// Component sprite sheets
#include "bn_sprite_items_skin_0.h"
#include "bn_sprite_items_hair_0.h"
#include "bn_sprite_items_hair_1.h"
#include "bn_sprite_items_eyes_0.h"
#include "bn_sprite_items_top_0.h"
#include "bn_sprite_items_bottom_0.h"

class Player
{
public:
    Player(const CharacterAppearance& appearance, const bn::fixed_point& start_pos);

    void update();

    const bn::fixed_point& position() const
    {
        return _pos;
    }

private:
    // Appearance snapshot from customization
    CharacterAppearance _appearance;

    // World state
    bn::fixed_point _pos;
    FacingDirection _direction;

    // Layered sprites (wrapped to avoid default-construct issues)
    bn::optional<bn::sprite_ptr> _body_sprite;
    bn::optional<bn::sprite_ptr> _eyes_sprite;
    bn::optional<bn::sprite_ptr> _top_sprite;
    bn::optional<bn::sprite_ptr> _bottom_sprite;
    bn::optional<bn::sprite_ptr> _hair_sprite;

    // Sprite items used (so we can set tiles each frame)
    const bn::sprite_item* _body_item   = nullptr;
    const bn::sprite_item* _eyes_item   = nullptr;
    const bn::sprite_item* _top_item    = nullptr;
    const bn::sprite_item* _bottom_item = nullptr;
    const bn::sprite_item* _hair_item   = nullptr;

    // Animation
    int _anim_counter = 0;
    int _idle_frame   = 0;   // 0..1
    int _walk_frame   = 0;   // 0..3
    bool _moving      = false;

    static constexpr bn::fixed k_speed = bn::fixed(1);

    void _handle_input();
    void _rebuild_sprites();     // choose sprite_items and create sprites
    void _apply_colors();        // recolor palettes
    void _update_animation();    // idle(2)/walk(4) anim step
    void _sync_sprites();        // position + frame index
};

#endif // PLAYER_H
