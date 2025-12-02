// ---------------------------------------------------------------------------
// customization_screen.cpp
// Terraria-like tabbed character customization with icon tabs and swatches.
// ---------------------------------------------------------------------------

#include "customization_screen.h"

#include "bn_keypad.h"
#include "bn_sprite_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_string.h"

#include "character_colors.h"

// Font
#include "common_fixed_8x8_sprite_font.h"

// Color swatch sprite (small grayscale square)
#include "bn_sprite_items_color_swatch.h"

// Tab icon sprites
#include "bn_sprite_items_tab_body.h"
#include "bn_sprite_items_tab_eyes.h"

#include "bn_sprite_items_tab_hair_style.h"
#include "bn_sprite_items_tab_hair_color.h"

#include "bn_sprite_items_tab_top_style.h"
#include "bn_sprite_items_tab_top_color.h"

#include "bn_sprite_items_tab_bottom_style.h"
#include "bn_sprite_items_tab_bottom_color.h"

// Character component sprite sheets (grayscale, 8 frames: 2 per direction)
#include "bn_sprite_items_skin_0.h"

#include "bn_sprite_items_hair_0.h"
#include "bn_sprite_items_hair_1.h"

#include "bn_sprite_items_eyes_0.h"

#include "bn_sprite_items_top_0.h"

#include "bn_sprite_items_bottom_0.h"

#include "bn_sprite_items_icon_border.h"
#include "bn_sprite_items_character_border.h"

namespace
{
    // ---------------------------------------------------------
    // Sprite option lists
    // ---------------------------------------------------------

    // Body: keep just one base for now (you can expand later)
    constexpr int k_body_type_count = 1;
    const bn::sprite_item* k_body_type_options[k_body_type_count] =
    {
        &bn::sprite_items::skin_0,
    };

    constexpr int k_hair_count = 2;
    const bn::sprite_item* k_hair_options[k_hair_count] =
    {
        &bn::sprite_items::hair_0,
        &bn::sprite_items::hair_1,
    };

    constexpr int k_eyes_count = 1;
    const bn::sprite_item* k_eyes_options[k_eyes_count] =
    {
        &bn::sprite_items::eyes_0,
    };

    constexpr int k_top_count = 1;
    const bn::sprite_item* k_top_options[k_top_count] =
    {
        &bn::sprite_items::top_0,
    };

    constexpr int k_bottom_count = 1;
    const bn::sprite_item* k_bottom_options[k_bottom_count] =
    {
        &bn::sprite_items::bottom_0,
    };

    // ---------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------

    int wrap_index(int index, int count)
    {
        if(count <= 0)
            return 0;

        if(index < 0)
        {
            index += count * ((-index / count) + 1);
        }

        index %= count;
        return index;
    }

    int tab_to_int(CustomizationTab tab)
    {
        return static_cast<int>(tab);
    }

    CustomizationTab int_to_tab(int value)
    {
        return static_cast<CustomizationTab>(value);
    }

    const bn::sprite_item* tab_icon_for(CustomizationTab tab)
    {
        switch(tab)
        {
            case CustomizationTab::BodyColor:
                return &bn::sprite_items::tab_body;

            case CustomizationTab::HairStyle:
                return &bn::sprite_items::tab_hair_style;
            case CustomizationTab::HairColor:
                return &bn::sprite_items::tab_hair_color;

            case CustomizationTab::EyesColor:
                return &bn::sprite_items::tab_eyes;

            case CustomizationTab::TopStyle:
                return &bn::sprite_items::tab_top_style;
            case CustomizationTab::TopColor:
                return &bn::sprite_items::tab_top_color;

            case CustomizationTab::BottomStyle:
                return &bn::sprite_items::tab_bottom_style;
            case CustomizationTab::BottomColor:
                return &bn::sprite_items::tab_bottom_color;

            default:
                return &bn::sprite_items::tab_body;
        }
    }

    void apply_ramp_to_palette(const ColorRamp& ramp, bn::sprite_palette_ptr& pal)
    {
        pal.set_color(1, ramp.c0);
        pal.set_color(2, ramp.c1);
        pal.set_color(3, ramp.c2);
        pal.set_color(4, ramp.c3);
    }

    // Use the first frame of the "down" idle (frame 0) for tiny previews.
    constexpr int k_preview_frame_index = 0;
}

// ---------------------------------------------------------------------------
// CustomizationScreen
// ---------------------------------------------------------------------------

CustomizationScreen::CustomizationScreen() :
    _preview(bn::fixed_point(-80, 0)),         // left side of screen
    _text_gen(common::fixed_8x8_sprite_font)
{
    _text_gen.set_left_alignment();

    // Default indices:
    _appearance.body_type_index = 0;
    _appearance.hair_index      = 0;
    _appearance.eyes_index      = 0;
    _appearance.top_index       = 0;
    _appearance.bottom_index    = 0;

    _apply_to_preview();
    _preview.set_direction(_appearance.direction);
    _preview.apply_colors(_appearance);

    _player_border = bn::sprite_items::character_border.create_sprite(-80, 0);
    _player_border->set_z_order(5);

    _refresh_ui();
}

// ---------------------------------------------------------------------------

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
        _move_tab(-1);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::r_pressed())
    {
        _move_tab(1);
        ui_needs_refresh = true;
    }

    // D-pad moves selection in the current tab grid
    if(bn::keypad::left_pressed())
    {
        _move_selection(0, -1);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::right_pressed())
    {
        _move_selection(0, 1);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::up_pressed())
    {
        _move_selection(-1, 0);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::down_pressed())
    {
        _move_selection(1, 0);
        ui_needs_refresh = true;
    }

    // Rotate character with A/B
    if(bn::keypad::a_pressed())
    {
        int d = static_cast<int>(_appearance.direction);
        d = (d + 1) % 4;
        _appearance.direction = static_cast<FacingDirection>(d);
        _preview.set_direction(_appearance.direction);
        _preview.set_scale(2);
        _preview.apply_colors(_appearance);
    }
    else if(bn::keypad::b_pressed())
    {
        int d = static_cast<int>(_appearance.direction);
        d = (d + 3) % 4; // -1 mod 4
        _appearance.direction = static_cast<FacingDirection>(d);
        _preview.set_direction(_appearance.direction);
        _preview.set_scale(2);
        _preview.apply_colors(_appearance);
    }

    if(ui_needs_refresh)
    {
        _refresh_ui();
    }

    // Submit character
    if(bn::keypad::start_pressed())
    {
        _done = true;
        return;
    }
}

// ---------------------------------------------------------------------------

void CustomizationScreen::_move_tab(int delta)
{
    int count = tab_to_int(CustomizationTab::COUNT);

    int index = tab_to_int(_current_tab);
    index = wrap_index(index + delta, count);

    _current_tab = int_to_tab(index);
}

// ---------------------------------------------------------------------------

void CustomizationScreen::_move_selection(int drow, int dcol)
{
    int& index = _tab_index_ref(_current_tab);

    int option_count = _tab_option_count(_current_tab);
    int cols         = _tab_grid_columns(_current_tab);
    int rows         = (option_count + cols - 1) / cols;

    int row = index / cols;
    int col = index % cols;

    // Special behaviour for horizontal movement
    if(dcol > 0 && drow == 0)             // move RIGHT
    {
        ++col;

        int new_index = row * cols + col;

        // If we stepped past the end of the row or past option_count:
        if(col >= cols || new_index >= option_count)
        {
            int next_row = row + 1;
            int first_next_index = next_row * cols;

            // If next row exists and has at least one item, go to its first
            if(first_next_index < option_count)
            {
                row = next_row;
                col = 0;
            }
            else
            {
                // No further rows: wrap to very first item
                row = 0;
                col = 0;
            }
        }
    }
    else if(dcol < 0 && drow == 0)        // move LEFT
    {
        if(col > 0)
        {
            --col;
        }
        else
        {
            if(row > 0)
            {
                // Go to the last valid item of the previous row
                --row;
                int last_index_prev_row = (row + 1) * cols - 1;
                if(last_index_prev_row >= option_count)
                {
                    last_index_prev_row = option_count - 1;
                }

                row = last_index_prev_row / cols;
                col = last_index_prev_row % cols;
            }
            else
            {
                // From first item, wrap to last item overall
                int last_index = option_count - 1;
                row = last_index / cols;
                col = last_index % cols;
            }
        }
    }
    else
    {
        // Vertical (or diagonal) movement: keep it simple & clamped
        row += drow;
        if(row < 0)
            row = 0;
        if(row >= rows)
            row = rows - 1;

        col += dcol;
        if(col < 0)
            col = 0;
        if(col >= cols)
            col = cols - 1;

        // Clamp to last valid index if this cell is "empty"
        int tmp_index = row * cols + col;
        if(tmp_index >= option_count)
        {
            tmp_index = option_count - 1;
            row = tmp_index / cols;
            col = tmp_index % cols;
        }
    }

    int new_index = row * cols + col;
    if(new_index < 0 || new_index >= option_count)
    {
        return;    // safety net
    }

    index = new_index;

    // Changes apply immediately
    if(_tab_is_style(_current_tab))
    {
        // Force sprites to rebuild
        _last_body_type_index = -1;
        _last_hair_index      = -1;
        _last_eyes_index      = -1;
        _last_top_index       = -1;
        _last_bottom_index    = -1;
    }
    else if(_tab_is_color(_current_tab))
    {
        _preview.apply_colors(_appearance);
    }
}

// ---------------------------------------------------------------------------
// Tab mapping helpers
// ---------------------------------------------------------------------------

bool CustomizationScreen::_tab_is_style(CustomizationTab tab) const
{
    switch(tab)
    {
        case CustomizationTab::HairStyle:
        case CustomizationTab::TopStyle:
        case CustomizationTab::BottomStyle:
            return true;
        default:
            return false;
    }
}

bool CustomizationScreen::_tab_is_color(CustomizationTab tab) const
{
    switch(tab)
    {
        case CustomizationTab::BodyColor:
        case CustomizationTab::HairColor:
        case CustomizationTab::EyesColor:
        case CustomizationTab::TopColor:
        case CustomizationTab::BottomColor:
            return true;
        default:
            return false;
    }
}

int CustomizationScreen::_tab_option_count(CustomizationTab tab) const
{
    switch(tab)
    {
        case CustomizationTab::BodyColor:
            return k_skin_color_count;

        case CustomizationTab::HairStyle:
            return k_hair_count;
        case CustomizationTab::HairColor:
            return k_feature_color_count;

        case CustomizationTab::EyesColor:
            return k_feature_color_count;

        case CustomizationTab::TopStyle:
            return k_top_count;
        case CustomizationTab::TopColor:
            return k_feature_color_count;

        case CustomizationTab::BottomStyle:
            return k_bottom_count;
        case CustomizationTab::BottomColor:
            return k_feature_color_count;

        default:
            return 1;
    }
}

int CustomizationScreen::_tab_grid_columns(CustomizationTab tab) const
{
    switch(tab)
    {
        case CustomizationTab::BodyColor:
            return 3;   // pale / tan / dark
        case CustomizationTab::HairStyle:
        case CustomizationTab::TopStyle:
        case CustomizationTab::BottomStyle:
            return 4;   // 4xN grid for styles
        case CustomizationTab::HairColor:
        case CustomizationTab::EyesColor:
        case CustomizationTab::TopColor:
        case CustomizationTab::BottomColor:
            return 5;   // 5 feature colors
        default:
            return 4;
    }
}

int& CustomizationScreen::_tab_index_ref(CustomizationTab tab)
{
    switch(tab)
    {
        case CustomizationTab::BodyColor:
            return _appearance.body_color_index;

        case CustomizationTab::HairStyle:
            return _appearance.hair_index;
        case CustomizationTab::HairColor:
            return _appearance.hair_color_index;

        case CustomizationTab::EyesColor:
            return _appearance.eyes_color_index;

        case CustomizationTab::TopStyle:
            return _appearance.top_index;
        case CustomizationTab::TopColor:
            return _appearance.top_color_index;

        case CustomizationTab::BottomStyle:
            return _appearance.bottom_index;
        case CustomizationTab::BottomColor:
            return _appearance.bottom_color_index;

        default:
            return _appearance.body_color_index; // fallback
    }
}

int CustomizationScreen::_tab_current_index(CustomizationTab tab) const
{
    switch(tab)
    {
        case CustomizationTab::BodyColor:
            return _appearance.body_color_index;

        case CustomizationTab::HairStyle:
            return _appearance.hair_index;
        case CustomizationTab::HairColor:
            return _appearance.hair_color_index;

        case CustomizationTab::EyesColor:
            return _appearance.eyes_color_index;

        case CustomizationTab::TopStyle:
            return _appearance.top_index;
        case CustomizationTab::TopColor:
            return _appearance.top_color_index;

        case CustomizationTab::BottomStyle:
            return _appearance.bottom_index;
        case CustomizationTab::BottomColor:
            return _appearance.bottom_color_index;

        default:
            return 0;
    }
}

// ---------------------------------------------------------------------------
// Preview application
// ---------------------------------------------------------------------------

void CustomizationScreen::_apply_to_preview()
{
    bool indices_changed =
        _appearance.body_type_index != _last_body_type_index ||
        _appearance.hair_index      != _last_hair_index      ||
        _appearance.eyes_index      != _last_eyes_index      ||
        _appearance.top_index       != _last_top_index       ||
        _appearance.bottom_index    != _last_bottom_index;

    bool colors_changed =
        _appearance.body_color_index   != _last_body_color_index   ||
        _appearance.hair_color_index   != _last_hair_color_index   ||
        _appearance.eyes_color_index   != _last_eyes_color_index   ||
        _appearance.top_color_index    != _last_top_color_index    ||
        _appearance.bottom_color_index != _last_bottom_color_index;

    if(!indices_changed && !colors_changed)
        return;

    if(indices_changed)
    {
        _last_body_type_index = _appearance.body_type_index;
        _last_hair_index      = _appearance.hair_index;
        _last_eyes_index      = _appearance.eyes_index;
        _last_top_index       = _appearance.top_index;
        _last_bottom_index    = _appearance.bottom_index;

        const bn::sprite_item* body   = k_body_type_options[0];  // single style for now
        const bn::sprite_item* hair   = k_hair_options[_appearance.hair_index];
        const bn::sprite_item* eyes   = k_eyes_options[_appearance.eyes_index];
        const bn::sprite_item* top    = k_top_options[_appearance.top_index];
        const bn::sprite_item* bottom = k_bottom_options[_appearance.bottom_index];

        _preview.set_layers(body, eyes, top, bottom, hair);
    }

    if(colors_changed || indices_changed)
    {
        _last_body_color_index   = _appearance.body_color_index;
        _last_hair_color_index   = _appearance.hair_color_index;
        _last_eyes_color_index   = _appearance.eyes_color_index;
        _last_top_color_index    = _appearance.top_color_index;
        _last_bottom_color_index = _appearance.bottom_color_index;

        _preview.apply_colors(_appearance);
    }

    _preview.set_scale(2);
}

// ---------------------------------------------------------------------------
// UI rendering
// ---------------------------------------------------------------------------

void CustomizationScreen::_refresh_ui()
{
    _text_sprites.clear();
    _option_sprites.clear();

    // ---------------------------------------------------------
    // Tab bar at the top (icons)
    // ---------------------------------------------------------

    const int tab_base_y = -60;
    const int tab_base_x = -70;
    const int tab_step_x = 20;

    int tab_count = tab_to_int(CustomizationTab::COUNT);

    for(int i = 0; i < tab_count; ++i)
    {
        CustomizationTab tab = int_to_tab(i);
        const bn::sprite_item* icon_item = tab_icon_for(tab);

        int x = tab_base_x + i * tab_step_x;
        int y = tab_base_y;

        bn::sprite_ptr icon = icon_item->create_sprite(x, y);

        if(tab == _current_tab)
        {
            // Highlight current tab: nudge it up a bit
            icon.set_y(y - 4);
        }

        _option_sprites.push_back(bn::move(icon));
    }

    // ---------------------------------------------------------
    // Info text (index / total)
    // ---------------------------------------------------------

    int current_index = _tab_current_index(_current_tab);
    int option_count  = _tab_option_count(_current_tab);

    bn::string<16> info;
    info += bn::to_string<4>(current_index + 1);
    info += "/";
    info += bn::to_string<4>(option_count);

    // ---------------------------------------------------------
    // Grid of options on the right
    // ---------------------------------------------------------

    int cols = _tab_grid_columns(_current_tab);
    int rows = (option_count + cols - 1) / cols;

    const int grid_start_x = -16;   // right-side area
    const int grid_start_y = -16;
    const int cell_w       = 28;
    const int cell_h       = 24;

    for(int i = 0; i < option_count; ++i)
    {
        int row = i / cols;
        int col = i % cols;

        int x = grid_start_x + col * cell_w;
        int y = grid_start_y + row * cell_h;

        int sprite_y = y;

        const bn::sprite_item* item = nullptr;
        const ColorRamp* ramp = nullptr;

        switch(_current_tab)
        {
            // BODY COLOR: keep the swatch for skin color (3 options)
            case CustomizationTab::BodyColor:
            {
                item = &bn::sprite_items::color_swatch;
                ramp = &get_skin_ramp(i);
                break;
            }

            // HAIR STYLE: preview hair style with current hair color
            case CustomizationTab::HairStyle:
            {
                item = k_hair_options[i];
                ramp = &get_feature_ramp(_appearance.hair_color_index);
                sprite_y += 4;
                break;
            }
            // HAIR COLOR: show the actual current hair style, recolored for each option
            case CustomizationTab::HairColor:
            {
                item = k_hair_options[_appearance.hair_index];
                ramp = &get_feature_ramp(i);
                sprite_y += 4;
                break;
            }

            // EYES COLOR: show the actual eyes sprite in each color
            case CustomizationTab::EyesColor:
            {
                item = k_eyes_options[0];
                ramp = &get_feature_ramp(i);
                sprite_y += 1;
                break;
            }

            // TOP STYLE: actual top sprites with current top color
            case CustomizationTab::TopStyle:
            {
                item = k_top_options[i];
                ramp = &get_feature_ramp(_appearance.top_color_index);
                sprite_y -= 2;
                break;
            }
            // TOP COLOR: current top style in each color
            case CustomizationTab::TopColor:
            {
                item = k_top_options[_appearance.top_index];
                ramp = &get_feature_ramp(i);
                sprite_y -= 2;
                break;
            }

            // BOTTOM STYLE: actual bottom sprites with current bottom color
            case CustomizationTab::BottomStyle:
            {
                item = k_bottom_options[i];
                ramp = &get_feature_ramp(_appearance.bottom_color_index);
                sprite_y -= 6;
                break;
            }
            // BOTTOM COLOR: current bottom style in each color
            case CustomizationTab::BottomColor:
            {
                item = k_bottom_options[_appearance.bottom_index];
                ramp = &get_feature_ramp(i);
                sprite_y -= 6;
                break;
            }

            default:
                break;
        }

        if(!item || !ramp)
            continue;

        bn::sprite_ptr s = item->create_sprite(x, sprite_y);

        s.set_tiles(item->tiles_item(), 0);

        bn::sprite_palette_ptr pal = item->palette_item().create_palette();
        s.set_palette(pal);

        apply_ramp_to_palette(*ramp, pal);

        // Border position matches the UI sprite
        bn::sprite_ptr border = bn::sprite_items::icon_border.create_sprite(x, y);

        if(i == current_index)
        {
            // Highlight current selection: nudge up a bit
            border.set_y(y - 4);
            s.set_y(sprite_y - 4);
        }

        // Ensure it draws *behind* the option sprite
        border.set_bg_priority(3);
        s.set_bg_priority(2);

        // Store border before sprite
        _option_sprites.push_back(bn::move(border));
        _option_sprites.push_back(bn::move(s));
    }
}
