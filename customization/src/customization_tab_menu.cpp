#include "customization_tab_menu.h"

#include "bn_sprite_item.h"

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
}

void CustomizationTabMenu::move(int delta)
{
    const int count = tab_to_int(CustomizationTab::COUNT);

    int index = tab_to_int(_current_tab);
    index = wrap_index(index + delta, count);

    _current_tab = int_to_tab(index);
}

void CustomizationTabMenu::draw(bn::vector<bn::sprite_ptr, 64>& option_sprites) const
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
            // Highlight current tab: nudge it up a bit
            icon.set_y(y - 4);
        }

        option_sprites.push_back(bn::move(icon));
    }
}
