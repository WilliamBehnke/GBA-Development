#include "customization_selection_grid.h"

#include "bn_sprite_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_string.h"
#include "bn_assert.h"

#include "character_colors.h"
#include "character_assets.h"

#include "bn_sprite_items_color_swatch.h"
#include "bn_sprite_items_icon_border.h"

namespace
{
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

} // namespace

// ---------------------------------------------------------------------------
// Classification
// ---------------------------------------------------------------------------

bool CustomizationSelectionGrid::is_style_tab(CustomizationTab tab)
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

bool CustomizationSelectionGrid::is_color_tab(CustomizationTab tab)
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

// ---------------------------------------------------------------------------
// Counts / grid layout
// ---------------------------------------------------------------------------

int CustomizationSelectionGrid::option_count(CustomizationTab tab)
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

int CustomizationSelectionGrid::grid_columns(CustomizationTab tab)
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
            return 5;   // grid columns for feature colors
        default:
            return 4;
    }
}

// ---------------------------------------------------------------------------
// Index mapping
// ---------------------------------------------------------------------------

int CustomizationSelectionGrid::current_index(const CharacterAppearance& appearance,
                                              CustomizationTab tab)
{
    switch(tab)
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

void CustomizationSelectionGrid::set_index(CharacterAppearance& appearance,
                                           CustomizationTab tab,
                                           int index)
{
    switch(tab)
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

// ---------------------------------------------------------------------------
// Movement
// ---------------------------------------------------------------------------

bool CustomizationSelectionGrid::move_selection(CustomizationTab tab,
                                                CharacterAppearance& appearance,
                                                int drow, int dcol)
{
    int index = current_index(appearance, tab);

    const int option_count_value = option_count(tab);
    const int cols = grid_columns(tab);
    const int rows = (option_count_value + cols - 1) / cols;

    int row = index / cols;
    int col = index % cols;

    // Special behaviour for horizontal movement
    if(dcol > 0 && drow == 0)             // move RIGHT
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
    else
    {
        // Vertical (or diagonal) movement: keep it simple & clamped
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

    set_index(appearance, tab, new_index);
    return true;
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

void CustomizationSelectionGrid::draw(CustomizationTab tab,
                                      const CharacterAppearance& appearance,
                                      bn::vector<bn::sprite_ptr, 64>& option_sprites)
{
    const int current = current_index(appearance, tab);
    const int count   = option_count(tab);

    const int cols = grid_columns(tab);
    const int rows = (count + cols - 1) / cols;

    const int grid_start_x = -16;   // right-side area
    const int grid_start_y = -16;
    const int cell_w       = 28;
    const int cell_h       = 24;

    for(int i = 0; i < count; ++i)
    {
        const int row = i / cols;
        const int col = i % cols;

        const int x = grid_start_x + col * cell_w;
        const int y = grid_start_y + row * cell_h;

        TabOptionVisual visual = compute_tab_option_visual(tab, i, appearance);
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

        option_sprites.push_back(bn::move(border));
        option_sprites.push_back(bn::move(s));
    }
}
