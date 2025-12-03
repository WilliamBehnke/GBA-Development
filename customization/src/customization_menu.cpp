#include "customization_menu.h"

#include "bn_sprite_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_string.h"
#include "bn_assert.h"

#include "character_colors.h"
#include "character_assets.h"

#include "bn_sprite_items_color_swatch.h"
#include "bn_sprite_items_icon_border.h"

#include "bn_sprite_items_tab_body.h"
#include "bn_sprite_items_tab_eyes.h"
#include "bn_sprite_items_tab_hair_style.h"
#include "bn_sprite_items_tab_hair_color.h"
#include "bn_sprite_items_tab_top_style.h"
#include "bn_sprite_items_tab_top_color.h"
#include "bn_sprite_items_tab_bottom_style.h"
#include "bn_sprite_items_tab_bottom_color.h"

namespace
{
    int wrap_index(int index, int count)
    {
        if(count <= 0)
        {
            return 0;
        }

        if(index < 0)
        {
            index += count * ((-index / count) + 1);
        }

        return index % count;
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

    int grid_columns_for(CustomizationTab tab)
    {
        switch(tab)
        {
            case CustomizationTab::BodyColor:
                return 3;
            case CustomizationTab::HairStyle:
            case CustomizationTab::TopStyle:
            case CustomizationTab::BottomStyle:
                return 4;
            case CustomizationTab::HairColor:
            case CustomizationTab::EyesColor:
            case CustomizationTab::TopColor:
            case CustomizationTab::BottomColor:
            default:
                return 5;
        }
    }

    struct TabOptionVisual
    {
        const bn::sprite_item* item = nullptr;
        const ColorRamp* ramp = nullptr;
        int sprite_y_offset = 0;

        bool valid() const
        {
            return item && ramp;
        }
    };

    TabOptionVisual compute_tab_option_visual(CustomizationTab current_tab,
                                              int option_index,
                                              const CharacterAppearance& appearance)
    {
        TabOptionVisual result;

        switch(current_tab)
        {
            // BODY COLOR: keep the swatch for skin color (3 options)
            case CustomizationTab::BodyColor:
            {
                result.item = &bn::sprite_items::color_swatch;
                result.ramp = &get_skin_ramp(static_cast<BodyColor>(option_index));
                result.sprite_y_offset = 0;
                break;
            }
            // HAIR STYLE: preview hair style with current hair color
            case CustomizationTab::HairStyle:
            {
                result.item = k_hair_options[appearance.hair_index];
                result.ramp = &get_feature_ramp(appearance.hair_color);
                result.sprite_y_offset = 4;
                break;
            }
            // HAIR COLOR: show the actual current hair style, recolored for each option
            case CustomizationTab::HairColor:
            {
                result.item = k_hair_options[appearance.hair_index];
                result.ramp = &get_feature_ramp(static_cast<FeatureColor>(option_index));
                result.sprite_y_offset = 4;
                break;
            }
            // EYES COLOR: show the actual eyes sprite in each color
            case CustomizationTab::EyesColor:
            {
                BN_ASSERT(k_eyes_count > 0, "Eyes options should not be empty");
                result.item = k_eyes_options[0];
                result.ramp = &get_feature_ramp(static_cast<FeatureColor>(option_index));
                result.sprite_y_offset = 1;
                break;
            }
            // TOP STYLE: actual top sprites with current top color
            case CustomizationTab::TopStyle:
            {
                result.item = k_top_options[appearance.top_index];
                result.ramp = &get_feature_ramp(appearance.top_color);
                result.sprite_y_offset = -2;
                break;
            }
            // TOP COLOR: current top style in each color
            case CustomizationTab::TopColor:
            {
                result.item = k_top_options[appearance.top_index];
                result.ramp = &get_feature_ramp(static_cast<FeatureColor>(option_index));
                result.sprite_y_offset = -2;
                break;
            }
            // BOTTOM STYLE: actual bottom sprites with current bottom color
            case CustomizationTab::BottomStyle:
            {
                result.item = k_bottom_options[appearance.bottom_index];
                result.ramp = &get_feature_ramp(appearance.bottom_color);
                result.sprite_y_offset = -6;
                break;
            }
            // BOTTOM COLOR: current bottom style in each color
            case CustomizationTab::BottomColor:
            {
                result.item = k_bottom_options[appearance.bottom_index];
                result.ramp = &get_feature_ramp(static_cast<FeatureColor>(option_index));
                result.sprite_y_offset = -6;
                break;
            }
            default:
                break;
        }

        return result;
    }

    constexpr int k_preview_frame_index = 0;

}

void CustomizationMenu::move_tab(int delta)
{
    const int count = static_cast<int>(CustomizationTab::COUNT);

    int index = static_cast<int>(_current_tab);
    index = wrap_index(index + delta, count);

    _current_tab = static_cast<CustomizationTab>(index);
}

bool CustomizationMenu::is_style_tab() const
{
    switch(_current_tab)
    {
        case CustomizationTab::HairStyle:
        case CustomizationTab::TopStyle:
        case CustomizationTab::BottomStyle:
            return true;
        default:
            return false;
    }
}

bool CustomizationMenu::is_color_tab() const
{
    switch(_current_tab)
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

int CustomizationMenu::_option_count() const
{
    switch(_current_tab)
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

int CustomizationMenu::_current_index(const CharacterAppearance& appearance) const
{
    switch(_current_tab)
    {
        case CustomizationTab::BodyColor:
            return static_cast<int>(appearance.body_color);
        case CustomizationTab::HairStyle:
            return appearance.hair_index;
        case CustomizationTab::HairColor:
            return static_cast<int>(appearance.hair_color);
        case CustomizationTab::EyesColor:
            return static_cast<int>(appearance.eyes_color);
        case CustomizationTab::TopStyle:
            return appearance.top_index;
        case CustomizationTab::TopColor:
            return static_cast<int>(appearance.top_color);
        case CustomizationTab::BottomStyle:
            return appearance.bottom_index;
        case CustomizationTab::BottomColor:
            return static_cast<int>(appearance.bottom_color);
        default:
            return 0;
    }
}

void CustomizationMenu::_set_index(CharacterAppearance& appearance, int index) const
{
    switch(_current_tab)
    {
        case CustomizationTab::BodyColor:
            appearance.body_color = static_cast<BodyColor>(index);
            break;
        case CustomizationTab::HairStyle:
            appearance.hair_index = index;
            break;
        case CustomizationTab::HairColor:
            appearance.hair_color = static_cast<FeatureColor>(index);
            break;
        case CustomizationTab::EyesColor:
            appearance.eyes_color = static_cast<FeatureColor>(index);
            break;
        case CustomizationTab::TopStyle:
            appearance.top_index = index;
            break;
        case CustomizationTab::TopColor:
            appearance.top_color = static_cast<FeatureColor>(index);
            break;
        case CustomizationTab::BottomStyle:
            appearance.bottom_index = index;
            break;
        case CustomizationTab::BottomColor:
            appearance.bottom_color = static_cast<FeatureColor>(index);
            break;
        default:
            break;
    }
}

bool CustomizationMenu::move_selection(CharacterAppearance& appearance,
                                       int drow, int dcol)
{
    int index = _current_index(appearance);

    const int option_count_value = _option_count();
    const int cols = grid_columns_for(_current_tab);
    const int rows = (option_count_value + cols - 1) / cols;

    int row = index / cols;
    int col = index % cols;

    if(dcol > 0 && drow == 0)            // move RIGHT
    {
        ++col;

        int new_index = row * cols + col;

        if(col >= cols || new_index >= option_count_value)
        {
            const int next_row = row + 1;
            const int first_next_index = next_row * cols;

            if(first_next_index < option_count_value)
            {
                row = next_row;
                col = 0;
            }
            else
            {
                row = 0;
                col = 0;
            }
        }
    }
    else if(dcol < 0 && drow == 0)       // move LEFT
    {
        if(col > 0)
        {
            --col;
        }
        else
        {
            if(row > 0)
            {
                --row;
                int last_index_prev_row = (row + 1) * cols - 1;
                if(last_index_prev_row >= option_count_value)
                {
                    last_index_prev_row = option_count_value - 1;
                }

                row = last_index_prev_row / cols;
                col = last_index_prev_row % cols;
            }
            else
            {
                const int last_index = option_count_value - 1;
                row = last_index / cols;
                col = last_index % cols;
            }
        }
    }
    else                                // move ertical (or diagonal)
    {
        row += drow;
        if(row < 0)
        {
            row = 0;
        }
        if(row >= rows)
        {
            row = rows - 1;
        }

        col += dcol;
        if(col < 0)
        {
            col = 0;
        }
        if(col >= cols)
        {
            col = cols - 1;
        }

        int tmp_index = row * cols + col;
        if(tmp_index >= option_count_value)
        {
            tmp_index = option_count_value - 1;
            row = tmp_index / cols;
            col = tmp_index % cols;
        }
    }

    const int new_index = row * cols + col;
    if(new_index < 0 || new_index >= option_count_value || new_index == index)
    {
        return false;
    }

    _set_index(appearance, new_index);
    return true;
}

void CustomizationMenu::draw(const CharacterAppearance& appearance)
{
    _option_sprites.clear();
    _draw_tabs();
    _draw_grid(appearance);
}

void CustomizationMenu::_draw_tabs()
{
    const int tab_base_y = -60;
    const int tab_base_x = -70;
    const int tab_step_x = 20;

    const int tab_count = static_cast<int>(CustomizationTab::COUNT);

    for(int i = 0; i < tab_count; ++i)
    {
        CustomizationTab tab = static_cast<CustomizationTab>(i);
        const bn::sprite_item* icon_item = tab_icon_for(tab);

        const int x = tab_base_x + i * tab_step_x;
        const int y = tab_base_y;

        bn::sprite_ptr icon = icon_item->create_sprite(x, y);

        if(tab == _current_tab)
        {
            icon.set_y(y - 4);
        }

        _option_sprites.push_back(bn::move(icon));
    }
}

void CustomizationMenu::_draw_grid(const CharacterAppearance& appearance)
{
    const int current = _current_index(appearance);
    const int count   = _option_count();

    const int cols = grid_columns_for(_current_tab);
    const int rows = (count + cols - 1) / cols;

    const int grid_start_x = -16;
    const int grid_start_y = -16;
    const int cell_w       = 28;
    const int cell_h       = 24;

    for(int i = 0; i < count; ++i)
    {
        const int row = i / cols;
        const int col = i % cols;

        const int x = grid_start_x + col * cell_w;
        const int y = grid_start_y + row * cell_h;

        TabOptionVisual visual = compute_tab_option_visual(_current_tab, i, appearance);
        if(!visual.valid())
        {
            continue;
        }

        int sprite_y = y + visual.sprite_y_offset;

        bn::sprite_ptr s = visual.item->create_sprite(x, sprite_y);
        s.set_tiles(visual.item->tiles_item(), k_preview_frame_index);

        bn::sprite_palette_ptr pal = visual.item->palette_item().create_palette();
        s.set_palette(pal);

        visual.ramp->apply_ramp_to_palette(pal);

        bn::sprite_ptr border = bn::sprite_items::icon_border.create_sprite(x, y);

        if(i == current)
        {
            border.set_y(y - 4);
            s.set_y(sprite_y - 4);
        }

        border.set_bg_priority(3);
        s.set_bg_priority(2);

        _option_sprites.push_back(bn::move(border));
        _option_sprites.push_back(bn::move(s));
    }
}
