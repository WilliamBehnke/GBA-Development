#include "upgrade_screen.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_math.h"

UpgradeScreen::UpgradeScreen(UpgradeGraph& graph, bn::sprite_text_generator& text_gen) :
    _graph(graph),
    _text_gen(text_gen),
    _bg(bn::regular_bg_items::upgrade_bg.create_bg(0, 0)),
    _cursor_sprite(bn::sprite_items::cursor.create_sprite(0, 0))
{
    _text_gen.set_center_alignment();

    _init_inventory();
    _create_node_sprites();
    _update_cursor_position();
    _update_text_panel();
}

void UpgradeScreen::_init_inventory()
{
    _inventory.clear();

    // Example starting inventory:
    // (tweak counts and types to taste)
    _inventory.push_back({ UpgradeType::HpUp,      3 });
    _inventory.push_back({ UpgradeType::AttackUp,  2 });
    _inventory.push_back({ UpgradeType::DefenseUp, 2 });

    _inventory_selected_index = 0;
}

// ----------------------------------------------------------
// Node sprite creation / updates
// ----------------------------------------------------------

void UpgradeScreen::_create_node_sprites()
{
    _node_sprites.clear();

    const auto& nodes = _graph.nodes();
    constexpr int CELL_SIZE = 16;

    for(const UpgradeNode& node : nodes)
    {
        int gx = node.grid_pos.x().integer();
        int gy = node.grid_pos.y().integer();

        int x = gx * CELL_SIZE;
        int y = gy * CELL_SIZE;

        // Placeholder; actual icon decided in _update_node_sprites():
        bn::sprite_ptr sp = bn::sprite_items::upgrade_locked.create_sprite(x, y);

        _node_sprites.push_back(bn::move(sp));
    }

    _update_node_sprites();
}

void UpgradeScreen::_update_node_sprites()
{
    const auto& nodes = _graph.nodes();
    constexpr int CELL_SIZE = 16;

    for(int i = 0, limit = nodes.size(); i < limit; ++i)
    {
        const UpgradeNode& node = nodes[i];

        int gx = node.grid_pos.x().integer();
        int gy = node.grid_pos.y().integer();
        int x = gx * CELL_SIZE;
        int y = gy * CELL_SIZE;

        // Create a fresh sprite with the correct item:
        bn::sprite_ptr sprite = bn::sprite_items::upgrade_blank.create_sprite(x, y);

        // Interpret node.type as "tile placed in this slot".
        if(node.type != UpgradeType::None)
        {
            // Slot has a tile: show tile icon
            switch(node.type)
            {
            case UpgradeType::HpUp:
                sprite = bn::sprite_items::upgrade_hp.create_sprite(x, y);
                break;
            case UpgradeType::AttackUp:
                sprite = bn::sprite_items::upgrade_attack.create_sprite(x, y);
                break;
            case UpgradeType::DefenseUp:
                sprite = bn::sprite_items::upgrade_defense.create_sprite(x, y);
                break;
            default:
                sprite = bn::sprite_items::upgrade_blank.create_sprite(x, y);
                break;
            }
        }
        else
        {
            // No tile yet: use blank / available / locked
            if(node.unlocked && node.available)
            {
                // Root slot or already-used path slot with no tile:
                sprite = bn::sprite_items::upgrade_available.create_sprite(x, y);
            }
            else if(_graph.can_unlock(node.id))
            {
                // Reachable slot (adjacent to a filled slot) with no tile:
                sprite = bn::sprite_items::upgrade_available.create_sprite(x, y);
            }
            else
            {
                // Not reachable yet:
                sprite = bn::sprite_items::upgrade_locked.create_sprite(x, y);
            }
        }
        _node_sprites[i] = bn::move(sprite);
    }
}

void UpgradeScreen::_update_cursor_position()
{
    const UpgradeNode& node = _graph.node(_selected_index);

    constexpr int CELL_SIZE = 16;

    int gx = node.grid_pos.x().integer();
    int gy = node.grid_pos.y().integer();

    int x = gx * CELL_SIZE;
    int y = gy * CELL_SIZE;

    _cursor_sprite.set_position(x, y);
    _cursor_sprite.set_z_order(-1);
}

// ----------------------------------------------------------
// Text panel
// ----------------------------------------------------------

void UpgradeScreen::_describe_node(const UpgradeNode& node,
                                   bn::string<64>& line1,
                                   bn::string<64>& line2) const
{
    if(node.type != UpgradeType::None)
    {
        // Slot has a tile
        switch(node.type)
        {
        case UpgradeType::HpUp:
            line1 = "HP tile";
            break;
        case UpgradeType::AttackUp:
            line1 = "Attack tile";
            break;
        case UpgradeType::DefenseUp:
            line1 = "Defense tile";
            break;
        default:
            line1 = "Tile";
            break;
        }

        line2 = "Placed";
        return;
    }

    // Empty slot
    line1 = "Empty slot";

    if(!_graph.can_unlock(node.id))
    {
        line2 = "Locked path";
    }
    else if(!_has_any_tiles_in_inventory()) {
        line2 = "Out of tiles";
    }
    else
    {
        line2 = "Press A to add tile";
    }
}

void UpgradeScreen::_update_text_panel()
{
    _text_sprites.clear();

    int base_y = -60;

    if(_mode == Mode::NavigateSlots)
    {
        const UpgradeNode& node = _graph.node(_selected_index);

        bn::string<64> line1;
        bn::string<64> line2;
        _describe_node(node, line1, line2);

        _text_gen.generate(0, base_y,      line1, _text_sprites);
        _text_gen.generate(0, base_y + 10, line2, _text_sprites);

        // Optional: small hint
        bn::string<64> hint = "D-pad move, A: tiles";
        _text_gen.generate(0, base_y + 20, hint, _text_sprites);
    }
    else // Mode::ChooseTile
    {
        if(_inventory.empty() || !_has_any_tiles_in_inventory())
        {
            bn::string<64> line1 = "No tiles available";
            bn::string<64> line2 = "Press B to cancel";

            _text_gen.generate(0, base_y,      line1, _text_sprites);
            _text_gen.generate(0, base_y + 10, line2, _text_sprites);
            return;
        }

        const TileStack& stack = _inventory[_inventory_selected_index];

        bn::string<64> line1 = "Choose tile:";
        bn::string<64> line2;

        switch(stack.type)
        {
        case UpgradeType::HpUp:
            line2 = "HP tile x";
            break;
        case UpgradeType::AttackUp:
            line2 = "Attack tile x";
            break;
        case UpgradeType::DefenseUp:
            line2 = "Defense tile x";
            break;
        default:
            line2 = "Tile x";
            break;
        }

        line2 += bn::to_string<4>(stack.count);

        bn::string<64> line3 = "D-Pad select, A place, B back";

        _text_gen.generate(0, base_y,      line1, _text_sprites);
        _text_gen.generate(0, base_y + 10, line2, _text_sprites);
        _text_gen.generate(0, base_y + 20, line3, _text_sprites);
    }
}

// ----------------------------------------------------------
// Inventory helpers
// ----------------------------------------------------------

bool UpgradeScreen::_has_any_tiles_in_inventory() const
{
    for(const TileStack& stack : _inventory)
    {
        if(stack.count > 0)
        {
            return true;
        }
    }
    return false;
}

void UpgradeScreen::_select_next_inventory_tile()
{
    if(_inventory.empty())
    {
        return;
    }

    int count = _inventory.size();
    for(int tries = 0; tries < count; ++tries)
    {
        _inventory_selected_index = (_inventory_selected_index + 1) % count;
        if(_inventory[_inventory_selected_index].count > 0)
        {
            return;
        }
    }
}

void UpgradeScreen::_select_prev_inventory_tile()
{
    if(_inventory.empty())
    {
        return;
    }

    int count = _inventory.size();
    for(int tries = 0; tries < count; ++tries)
    {
        _inventory_selected_index = (_inventory_selected_index - 1 + count) % count;
        if(_inventory[_inventory_selected_index].count > 0)
        {
            return;
        }
    }
}

bool UpgradeScreen::_current_slot_has_tile() const
{
    const UpgradeNode& node = _graph.node(_selected_index);
    return node.type != UpgradeType::None;
}

bool UpgradeScreen::_current_slot_can_accept_tile() const
{
    const UpgradeNode& node = _graph.node(_selected_index);

    // Only allow placing on empty, reachable slots
    if(node.type != UpgradeType::None)
    {
        return false;
    }

    return _graph.can_unlock(node.id);
}

// ----------------------------------------------------------
// Input handling
// ----------------------------------------------------------

void UpgradeScreen::_handle_input()
{
    if(_mode == Mode::NavigateSlots)
    {
        _handle_input_navigate();
    }
    else
    {
        _handle_input_choose_tile();
    }
}

void UpgradeScreen::_handle_input_navigate()
{
    if(bn::keypad::left_pressed())
    {
        _move_selection_left();
    }
    else if(bn::keypad::right_pressed())
    {
        _move_selection_right();
    }
    else if(bn::keypad::up_pressed())
    {
        _move_selection_up();
    }
    else if(bn::keypad::down_pressed())
    {
        _move_selection_down();
    }

    if(bn::keypad::a_pressed())
    {
        _enter_choose_tile_mode();
    }

    _update_text_panel();
}

void UpgradeScreen::_handle_input_choose_tile()
{
    if(bn::keypad::left_pressed())
    {
        _select_prev_inventory_tile();
    }
    else if(bn::keypad::right_pressed())
    {
        _select_next_inventory_tile();
    }

    if(bn::keypad::b_pressed())
    {
        // Cancel tile selection
        _mode = Mode::NavigateSlots;
        _update_text_panel();
        return;
    }

    if(bn::keypad::a_pressed())
    {
        _place_selected_tile();
    }

    _update_text_panel();
}

void UpgradeScreen::_enter_choose_tile_mode()
{
    if(!_current_slot_can_accept_tile())
    {
        return;
    }

    if(!_has_any_tiles_in_inventory())
    {
        return;
    }

    // Ensure we start on a stack that has at least 1
    if(_inventory[_inventory_selected_index].count <= 0)
    {
        _select_next_inventory_tile();
    }

    _mode = Mode::ChooseTile;
}

void UpgradeScreen::_place_selected_tile()
{
    if(_inventory.empty() || !_has_any_tiles_in_inventory())
    {
        _mode = Mode::NavigateSlots;
        return;
    }

    TileStack& stack = _inventory[_inventory_selected_index];
    if(stack.count <= 0)
    {
        return;
    }

    if(!_current_slot_can_accept_tile())
    {
        _mode = Mode::NavigateSlots;
        return;
    }

    UpgradeNode& node = _graph.node(_selected_index);

    // Place tile into this slot and consume one
    node.type = stack.type;
    node.unlocked = true;          // this slot is now "used" and opens neighbors
    stack.count--;

    _graph.update_availability();
    _update_node_sprites();

    _mode = Mode::NavigateSlots;
}

// ----------------------------------------------------------
// D-pad navigation between slots (N/E/S/W neighbors)
// ----------------------------------------------------------

void UpgradeScreen::_move_selection_up()
{
    const UpgradeNode& current = _graph.node(_selected_index);
    int neighbor_id = current.neighbor_id(NeighborDir::North);
    if(neighbor_id < 0)
    {
        return;
    }

    int ni = _graph.index_from_id(neighbor_id);
    if(ni < 0)
    {
        return;
    }

    _selected_index = ni;
    _update_cursor_position();
}

void UpgradeScreen::_move_selection_down()
{
    const UpgradeNode& current = _graph.node(_selected_index);
    int neighbor_id = current.neighbor_id(NeighborDir::South);
    if(neighbor_id < 0)
    {
        return;
    }

    int ni = _graph.index_from_id(neighbor_id);
    if(ni < 0)
    {
        return;
    }

    _selected_index = ni;
    _update_cursor_position();
}

void UpgradeScreen::_move_selection_left()
{
    const UpgradeNode& current = _graph.node(_selected_index);
    int neighbor_id = current.neighbor_id(NeighborDir::West);
    if(neighbor_id < 0)
    {
        return;
    }

    int ni = _graph.index_from_id(neighbor_id);
    if(ni < 0)
    {
        return;
    }

    _selected_index = ni;
    _update_cursor_position();
}

void UpgradeScreen::_move_selection_right()
{
    const UpgradeNode& current = _graph.node(_selected_index);
    int neighbor_id = current.neighbor_id(NeighborDir::East);
    if(neighbor_id < 0)
    {
        return;
    }

    int ni = _graph.index_from_id(neighbor_id);
    if(ni < 0)
    {
        return;
    }

    _selected_index = ni;
    _update_cursor_position();
}

// ----------------------------------------------------------
// Main update
// ----------------------------------------------------------

void UpgradeScreen::update()
{
    _handle_input();
}
