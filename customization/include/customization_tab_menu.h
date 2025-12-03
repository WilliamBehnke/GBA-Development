#ifndef CUSTOMIZATION_TAB_MENU_H
#define CUSTOMIZATION_TAB_MENU_H

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

// Tabs for the customization UI
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

class CustomizationTabMenu
{
public:
    CustomizationTabMenu() = default;

    CustomizationTab current_tab() const
    {
        return _current_tab;
    }

    void set_tab(CustomizationTab tab)
    {
        _current_tab = tab;
    }

    void move(int delta);

    void draw(bn::vector<bn::sprite_ptr, 64>& option_sprites) const;

private:
    CustomizationTab _current_tab = CustomizationTab::BodyColor;
};

#endif // CUSTOMIZATION_TAB_MENU_H
