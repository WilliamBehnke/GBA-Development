#include "customization_screen.h"

#include "bn_keypad.h"
#include "bn_string.h"

#include "character_colors.h"

#include "bn_sprite_items_icon_border.h"
#include "bn_sprite_items_character_border.h"

#include "character_assets.h"

// ---------------------------------------------------------------------------
// Ctor + lifecycle
// ---------------------------------------------------------------------------

CustomizationScreen::CustomizationScreen() : _preview(bn::fixed_point(-80, 0))
{
    _appearance.hair_index      = 0;
    _appearance.top_index       = 0;
    _appearance.bottom_index    = 0;

    _apply_to_preview();
    _preview.set_direction(_appearance.direction);
    _preview.apply_colors(_appearance);

    _player_border = bn::sprite_items::character_border.create_sprite(-80, 0);
    _player_border->set_z_order(5);

    _refresh_ui();
}

void CustomizationScreen::update()
{
    _handle_input();
    _apply_to_preview();
    _preview.update();
}

// ---------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------

void CustomizationScreen::_handle_input()
{
    bool ui_needs_refresh = false;

    // Bumpers switch tabs
    if(bn::keypad::l_pressed())
    {
        _tabs.move(-1);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::r_pressed())
    {
        _tabs.move(1);
        ui_needs_refresh = true;
    }

    // D-pad moves selection in the current tab grid
    CustomizationTab tab = _tabs.current_tab();

    auto handle_move = [&](int drow, int dcol)
    {
        bool changed = CustomizationSelectionGrid::move_selection(tab, _appearance, drow, dcol);
        if(!changed)
        {
            return;
        }

        ui_needs_refresh = true;

        if(CustomizationSelectionGrid::is_style_tab(tab))
        {
            _mark_style_sprites_dirty();
        }
        else if(CustomizationSelectionGrid::is_color_tab(tab))
        {
            _preview.apply_colors(_appearance);
        }
    };

    if(bn::keypad::left_pressed())
    {
        handle_move(0, -1);
    }
    else if(bn::keypad::right_pressed())
    {
        handle_move(0, 1);
    }
    else if(bn::keypad::up_pressed())
    {
        handle_move(-1, 0);
    }
    else if(bn::keypad::down_pressed())
    {
        handle_move(1, 0);
    }

    // Rotate character with A/B
    if(bn::keypad::a_pressed())
    {
        _rotate_character(1);
    }
    else if(bn::keypad::b_pressed())
    {
        _rotate_character(-1);   // -1 mod 4
    }

    if(ui_needs_refresh)
    {
        _refresh_ui();
    }

    if(bn::keypad::start_pressed())
    {
        _done = true;
    }
}

// ---------------------------------------------------------------------------

void CustomizationScreen::_rotate_character(int delta_steps)
{
    int d = static_cast<int>(_appearance.direction);
    d = (d + delta_steps + 4) % 4;
    _appearance.direction = static_cast<FacingDirection>(d);

    _preview.set_direction(_appearance.direction);
    _preview.set_scale(2);
    _preview.apply_colors(_appearance);
}

// ---------------------------------------------------------------------------
// Preview application
// ---------------------------------------------------------------------------

void CustomizationScreen::_mark_style_sprites_dirty()
{
    _last_hair_index      = -1;
    _last_top_index       = -1;
    _last_bottom_index    = -1;
}

void CustomizationScreen::_apply_to_preview()
{
    const bool indices_changed =
        _appearance.hair_index      != _last_hair_index      ||
        _appearance.top_index       != _last_top_index       ||
        _appearance.bottom_index    != _last_bottom_index;

    const bool colors_changed =
        !_last_body_color   || _appearance.body_color   != *_last_body_color   ||
        !_last_hair_color   || _appearance.hair_color   != *_last_hair_color   ||
        !_last_eyes_color   || _appearance.eyes_color   != *_last_eyes_color   ||
        !_last_top_color    || _appearance.top_color    != *_last_top_color    ||
        !_last_bottom_color || _appearance.bottom_color != *_last_bottom_color;

    if(!indices_changed && !colors_changed)
    {
        return;
    }

    if(indices_changed)
    {
        _last_hair_index      = _appearance.hair_index;
        _last_top_index       = _appearance.top_index;
        _last_bottom_index    = _appearance.bottom_index;

        const bn::sprite_item* body   = k_body_type_options[0];
        const bn::sprite_item* hair   = k_hair_options[_appearance.hair_index];
        const bn::sprite_item* eyes   = k_eyes_options[0];
        const bn::sprite_item* top    = k_top_options[_appearance.top_index];
        const bn::sprite_item* bottom = k_bottom_options[_appearance.bottom_index];

        _preview.set_layers(body, eyes, top, bottom, hair);
    }

    if(colors_changed || indices_changed)
    {
        _last_body_color   = _appearance.body_color;
        _last_hair_color   = _appearance.hair_color;
        _last_eyes_color   = _appearance.eyes_color;
        _last_top_color    = _appearance.top_color;
        _last_bottom_color = _appearance.bottom_color;

        _preview.apply_colors(_appearance);
    }

    _preview.set_scale(2);
}

// ---------------------------------------------------------------------------
// UI rendering
// ---------------------------------------------------------------------------

void CustomizationScreen::_refresh_ui()
{
    _option_sprites.clear();

    _tabs.draw(_option_sprites);
    CustomizationSelectionGrid::draw(_tabs.current_tab(), _appearance, _option_sprites);
}
