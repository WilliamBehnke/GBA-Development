#ifndef UPGRADE_SCREEN_H
#define UPGRADE_SCREEN_H

#include "upgrade_graph.h"

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_fixed_point.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"

// Slot icons:
#include "bn_sprite_items_upgrade_hp.h"
#include "bn_sprite_items_upgrade_attack.h"
#include "bn_sprite_items_upgrade_defense.h"
#include "bn_sprite_items_upgrade_blank.h"
#include "bn_sprite_items_upgrade_available.h"
#include "bn_sprite_items_upgrade_locked.h"

#include "bn_sprite_items_cursor.h"
#include "bn_regular_bg_items_upgrade_bg.h"

class UpgradeScreen
{
public:
    UpgradeScreen(UpgradeGraph& graph, bn::sprite_text_generator& text_gen);

    void update();

private:
    enum class Mode
    {
        NavigateSlots,
        ChooseTile
    };

    struct TileStack
    {
        UpgradeType type;
        int count;
    };

    UpgradeGraph& _graph;
    bn::sprite_text_generator& _text_gen;

    bn::regular_bg_ptr _bg;

    // One sprite per slot node
    bn::vector<bn::sprite_ptr, UpgradeGraph::max_nodes> _node_sprites;

    bn::sprite_ptr _cursor_sprite;
    bn::vector<bn::sprite_ptr, 16> _text_sprites;

    int _selected_index = 0;

    // Inventory of upgrade tiles
    bn::vector<TileStack, 8> _inventory;
    int _inventory_selected_index = 0;

    Mode _mode = Mode::NavigateSlots;

    void _init_inventory();

    void _create_node_sprites();
    void _update_node_sprites();
    void _update_cursor_position();
    void _update_text_panel();

    void _handle_input();

    void _handle_input_navigate();
    void _handle_input_choose_tile();

    void _move_selection_up();
    void _move_selection_down();
    void _move_selection_left();
    void _move_selection_right();

    void _enter_choose_tile_mode();
    void _place_selected_tile();

    bool _current_slot_has_tile() const;
    bool _current_slot_can_accept_tile() const;
    bool _has_any_tiles_in_inventory() const;

    void _select_next_inventory_tile();
    void _select_prev_inventory_tile();

    void _describe_node(const UpgradeNode& node,
                        bn::string<64>& line1,
                        bn::string<64>& line2) const;
};

#endif // UPGRADE_SCREEN_H
