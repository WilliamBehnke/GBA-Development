#ifndef CUSTOMIZATION_SCREEN_H
#define CUSTOMIZATION_SCREEN_H

#include "character_appearance.h"
#include "character_preview.h"
#include "customization_tab_menu.h"
#include "customization_selection_grid.h"

#include "bn_sprite_text_generator.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_optional.h"

class CustomizationScreen
{
public:
    CustomizationScreen();

    void update();

    bool done() const
    {
        return _done;
    }

    const CharacterAppearance& appearance() const
    {
        return _appearance;
    }

private:
    // Input + state
    void _handle_input();
    void _rotate_character(int delta_steps);

    // Rendering / preview
    void _apply_to_preview();
    void _refresh_ui();

    // Used when a style tab changes
    void _mark_style_sprites_dirty();

    CharacterAppearance _appearance;
    CharacterPreview _preview;

    CustomizationTabMenu _tabs;

    // Cache to avoid rebuilding sprites / palettes when nothing changed
    int _last_hair_index      = -1;
    int _last_top_index       = -1;
    int _last_bottom_index    = -1;

    bn::optional<BodyColor>    _last_body_color;
    bn::optional<FeatureColor> _last_hair_color;
    bn::optional<FeatureColor> _last_eyes_color;
    bn::optional<FeatureColor> _last_top_color;
    bn::optional<FeatureColor> _last_bottom_color;

    bn::vector<bn::sprite_ptr, 64>  _option_sprites;

    bn::optional<bn::sprite_ptr> _player_border;

    bool _done = false;
};

#endif // CUSTOMIZATION_SCREEN_H
