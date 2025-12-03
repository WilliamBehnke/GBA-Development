#ifndef CUSTOMIZATION_SELECTION_GRID_H
#define CUSTOMIZATION_SELECTION_GRID_H

#include "customization_tab_menu.h"
#include "character_appearance.h"

#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class CustomizationSelectionGrid
{
public:
    // Navigation helpers
    static bool is_style_tab(CustomizationTab tab);
    static bool is_color_tab(CustomizationTab tab);

    static int option_count(CustomizationTab tab);
    static int grid_columns(CustomizationTab tab);

    static int  current_index(const CharacterAppearance& appearance, CustomizationTab tab);
    static void set_index(CharacterAppearance& appearance, CustomizationTab tab, int index);

    // Returns true if selection changed
    static bool move_selection(CustomizationTab tab,
                               CharacterAppearance& appearance,
                               int drow, int dcol);

    // Draws the grid options (swatches / previews) and their borders
    static void draw(CustomizationTab tab,
                     const CharacterAppearance& appearance,
                     bn::vector<bn::sprite_ptr, 64>& option_sprites);
};

#endif // CUSTOMIZATION_SELECTION_GRID_H
