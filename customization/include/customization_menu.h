#ifndef CUSTOMIZATION_MENU_H
#define CUSTOMIZATION_MENU_H

#include "character_appearance.h"

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

// ---------------------------------------------------------------------------
// Customization tabs
// ---------------------------------------------------------------------------

enum class CustomizationTab : int
{
    BodyColor = 0,
    HairStyle,
    HairColor,
    EyesColor,
    TopStyle,
    TopColor,
    BottomStyle,
    BottomColor,

    COUNT
};

// ---------------------------------------------------------------------------
// CustomizationMenu
// Owns current tab, handles navigation + grid selection + drawing.
// ---------------------------------------------------------------------------

class CustomizationMenu
{
public:
    CustomizationMenu() = default;

    CustomizationTab current_tab() const
    {
        return _current_tab;
    }

    // Classification for current tab
    bool is_style_tab() const;
    bool is_color_tab() const;

    // Move selection within the current tab's grid.
    // Returns true if the CharacterAppearance changed.
    bool move_selection(CharacterAppearance& appearance, int drow, int dcol);

    // Move between tabs (L/R bumpers, etc.)
    void move_tab(int delta);

    // Drawing (clears and rebuilds internal sprites)
    void draw(const CharacterAppearance& appearance);

private:
    // Helpers based on _current_tab
    int _option_count() const;
    int _current_index(const CharacterAppearance& appearance) const;
    void _set_index(CharacterAppearance& appearance, int index) const;

    void _draw_tabs();
    void _draw_grid(const CharacterAppearance& appearance);

    CustomizationTab _current_tab = CustomizationTab::BodyColor;

    // Owned sprites for tabs + grid options
    bn::vector<bn::sprite_ptr, 64> _option_sprites;
};

#endif // CUSTOMIZATION_MENU_H
