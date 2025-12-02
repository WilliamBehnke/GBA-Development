#ifndef CUSTOMIZATION_SCREEN_H
#define CUSTOMIZATION_SCREEN_H

// ---------------------------------------------------------------------------
// customization_screen.h
// Terraria-style tabs + grid selector for style / color.
// ---------------------------------------------------------------------------

#include "character_appearance.h"
#include "character_preview.h"

#include "bn_sprite_text_generator.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

enum class CustomizationTab : int
{
    BodyColor = 0,    // Body color only (pale / tan / dark)
    HairStyle,
    HairColor,
    EyesColor,
    TopStyle,
    TopColor,
    BottomStyle,
    BottomColor,
    COUNT
};

class CustomizationScreen
{
public:
    CustomizationScreen();

    // Call once per frame from main loop
    void update();

    const CharacterAppearance& appearance() const
    {
        return _appearance;
    }

private:
    // Input + state
    void _handle_input();
    void _move_tab(int delta);
    void _move_selection(int drow, int dcol);

    // Data mapping
    bool _tab_is_style(CustomizationTab tab) const;
    bool _tab_is_color(CustomizationTab tab) const;

    int  _tab_option_count(CustomizationTab tab) const;
    int  _tab_grid_columns(CustomizationTab tab) const;

    int& _tab_index_ref(CustomizationTab tab);      // style or color index
    int  _tab_current_index(CustomizationTab tab) const;

    // Rendering / preview
    void _apply_to_preview();
    void _refresh_ui();

    CharacterAppearance _appearance;
    CharacterPreview _preview;
    CustomizationTab _current_tab = CustomizationTab::BodyColor;

    // Cache to avoid rebuilding sprites / palettes when nothing changed
    int _last_body_type_index = -1;
    int _last_hair_index      = -1;
    int _last_eyes_index      = -1;
    int _last_top_index       = -1;
    int _last_bottom_index    = -1;

    int _last_body_color_index   = -1;
    int _last_hair_color_index   = -1;
    int _last_eyes_color_index   = -1;
    int _last_top_color_index    = -1;
    int _last_bottom_color_index = -1;

    // UI sprites
    bn::sprite_text_generator _text_gen;
    bn::vector<bn::sprite_ptr, 128> _text_sprites;
    bn::vector<bn::sprite_ptr, 64>  _option_sprites;  // grid & tab visuals

    bn::optional<bn::sprite_ptr> _player_border;
};

#endif // CUSTOMIZATION_SCREEN_H
