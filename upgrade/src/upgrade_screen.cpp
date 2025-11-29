#include "upgrade_screen.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_math.h"

// ----------------------------------------------------------
// Helpers
// ----------------------------------------------------------

bn::fixed_point UpgradeScreen::_world_to_screen(const bn::fixed_point& world) const
{
    // Screen coords = world - camera, with (0,0) at screen center
    return bn::fixed_point(world.x() - _camera.x(), world.y() - _camera.y());
}

void UpgradeScreen::_apply_camera_to_nodes()
{
    const auto& nodes = _graph.nodes();

    for(int i = 0, limit = nodes.size(); i < limit; ++i)
    {
        const UpgradeNode& node = nodes[i];
        bn::fixed_point screen = _world_to_screen(node.grid_pos);

        int x = screen.x().integer();
        int y = screen.y().integer();

        _node_sprites[i].set_position(x, y);
    }
}

// ----------------------------------------------------------
// Ctor / init
// ----------------------------------------------------------

UpgradeScreen::UpgradeScreen(UpgradeGraph& graph, bn::sprite_text_generator& text_gen) :
    _graph(graph),
    _text_gen(text_gen),
    _bg(bn::regular_bg_items::upgrade_bg.create_bg(0, 0)),
    _cursor_sprite_16(bn::sprite_items::cursor_16.create_sprite(0, 0)),
    _cursor_sprite_24(bn::sprite_items::cursor_24.create_sprite(0, 0)),
    _camera(0, 0)
{
    _text_gen.set_center_alignment();

    _init_inventory();

    // Start camera on the first selected node:
    const UpgradeNode& start_node = _graph.node(_selected_index);
    _camera = start_node.grid_pos;

    _cursor_sprite_24.set_visible(false);   // only used on ability slots

    _create_node_sprites();
    _update_cursor_position();
    _update_text_panel();
}

void UpgradeScreen::_init_inventory()
{
    _inventory.clear();

    _inventory.push_back({ UpgradeType::HpUp,      3 });
    _inventory.push_back({ UpgradeType::AttackUp,  2 });
    _inventory.push_back({ UpgradeType::DefenseUp, 2 });
    _inventory.push_back({ UpgradeType::Ability,   1 });

    _inventory_selected_index = 0;

    _curse_charms = 1;
}

// ----------------------------------------------------------
// Node sprite creation / updates
// ----------------------------------------------------------

void UpgradeScreen::_create_node_sprites()
{
    _node_sprites.clear();

    const auto& nodes = _graph.nodes();

    for(const UpgradeNode& node : nodes)
    {
        bn::fixed_point screen = _world_to_screen(node.grid_pos);
        int x = screen.x().integer();
        int y = screen.y().integer();

        bn::sprite_ptr sp = bn::sprite_items::upgrade_locked.create_sprite(x, y);
        _node_sprites.push_back(bn::move(sp));
    }

    _update_node_sprites();
}

void UpgradeScreen::_update_node_sprites()
{
    const auto& nodes = _graph.nodes();

    for(int i = 0, limit = nodes.size(); i < limit; ++i)
    {
        const UpgradeNode& node = nodes[i];

        bn::fixed_point screen = _world_to_screen(node.grid_pos);
        int x = screen.x().integer();
        int y = screen.y().integer();

        bn::sprite_ptr sprite = bn::sprite_items::upgrade_locked.create_sprite(x, y);

        // 1) Slot has a tile
        if(node.type != UpgradeType::None)
        {
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
            case UpgradeType::Ability:
                sprite = bn::sprite_items::upgrade_ability.create_sprite(x, y);
                break;
            default:
                sprite = bn::sprite_items::upgrade_blank.create_sprite(x, y);
                break;
            }
        }
        // 2) Cursed non-ability slot
        else if(node.is_cursed && !node.curse_cleared && !node.is_ability_slot)
        {
            sprite = bn::sprite_items::upgrade_cursed.create_sprite(x, y);
        }
        // 3) Empty ability-only slot: locked/available
        else if(node.is_ability_slot)
        {
            if(_graph.can_unlock(node.id))
            {
                sprite = bn::sprite_items::upgrade_ability_available.create_sprite(x, y);
            }
            else
            {
                sprite = bn::sprite_items::upgrade_ability_locked.create_sprite(x, y);
            }
        }
        // 4) Empty regular slot: available / locked
        else
        {
            if(_graph.can_unlock(node.id))
            {
                sprite = bn::sprite_items::upgrade_available.create_sprite(x, y);
            }
            else
            {
                sprite = bn::sprite_items::upgrade_locked.create_sprite(x, y);
            }
        }

        _node_sprites[i] = bn::move(sprite);
    }
}

// ----------------------------------------------------------
// Camera + cursor
// ----------------------------------------------------------

void UpgradeScreen::_update_cursor_position()
{
    const UpgradeNode& node = _graph.node(_selected_index);

    // ------------------------------------------------------
    // 1) Adjust camera only when cursor nears screen edges
    // ------------------------------------------------------

    // Screen is 240x160 => half sizes:
    constexpr bn::fixed screen_half_w = 120;
    constexpr bn::fixed screen_half_h = 80;

    // Dead-zone margins: inside this box we don't move the camera.
    // Tweak these to taste (bigger margin = less camera movement).
    constexpr bn::fixed margin_x = 60;   // horizontal safe zone
    constexpr bn::fixed margin_y = 40;   // vertical safe zone

    // Current screen position of the selected node with the *old* camera:
    bn::fixed_point screen = _world_to_screen(node.grid_pos);

    bn::fixed new_cam_x = _camera.x();
    bn::fixed new_cam_y = _camera.y();

    // Right edge
    bn::fixed right_limit = screen_half_w - margin_x;
    if(screen.x() > right_limit)
    {
        // Shift camera so node is back at right_limit
        bn::fixed delta = screen.x() - right_limit;
        new_cam_x += delta;
    }
    // Left edge
    bn::fixed left_limit = -screen_half_w + margin_x;
    if(screen.x() < left_limit)
    {
        bn::fixed delta = screen.x() - left_limit;
        new_cam_x += delta;
    }

    // Bottom edge
    bn::fixed bottom_limit = screen_half_h - margin_y;
    if(screen.y() > bottom_limit)
    {
        bn::fixed delta = screen.y() - bottom_limit;
        new_cam_y += delta;
    }
    // Top edge
    bn::fixed top_limit = -screen_half_h + margin_y;
    if(screen.y() < top_limit)
    {
        bn::fixed delta = screen.y() - top_limit;
        new_cam_y += delta;
    }

    // ------------------------------------------------------
    // 2) Clamp camera to 512x512 map bounds
    // ------------------------------------------------------

    constexpr bn::fixed map_half = 256;        // 512 / 2
    bn::fixed min_x = -map_half + screen_half_w;
    bn::fixed max_x =  map_half - screen_half_w;
    bn::fixed min_y = -map_half + screen_half_h;
    bn::fixed max_y =  map_half - screen_half_h;

    new_cam_x = bn::clamp(new_cam_x, min_x, max_x);
    new_cam_y = bn::clamp(new_cam_y, min_y, max_y);

    _camera.set_x(new_cam_x);
    _camera.set_y(new_cam_y);

    // ------------------------------------------------------
    // 3) Apply camera to node sprites
    // ------------------------------------------------------

    // Scroll the 512x512 background with the camera.
    // If the direction feels inverted, flip the signs.
    _bg.set_position(-_camera.x().integer(), -_camera.y().integer());

    _apply_camera_to_nodes();

    // Recompute node screen position using the updated camera:
    screen = _world_to_screen(node.grid_pos);
    int x = screen.x().integer();
    int y = screen.y().integer();

    // ------------------------------------------------------
    // 4) Cursor: 16x16 vs 24x24
    // ------------------------------------------------------

    if(node.is_ability_slot)
    {
        // Show the 24×24 cursor
        _cursor_sprite_16.set_visible(false);

        _cursor_sprite_24.set_visible(true);
        _cursor_sprite_24.set_position(x, y);
        _cursor_sprite_24.set_z_order(-1);
    }
    else
    {
        // Show the normal cursor
        _cursor_sprite_24.set_visible(false);

        _cursor_sprite_16.set_visible(true);
        _cursor_sprite_16.set_position(x, y);
        _cursor_sprite_16.set_z_order(-1);
    }
}

// ----------------------------------------------------------
// Text panel
// ----------------------------------------------------------

void UpgradeScreen::_describe_node(const UpgradeNode& node,
                                   bn::string<64>& line1,
                                   bn::string<64>& line2) const
{
    // Cursed slots first (non-ability)
    if(node.is_cursed && !node.curse_cleared && !node.is_ability_slot)
    {
        line1 = "Cursed slot";

        if(_curse_charms > 0)
        {
            line2 = "A: cleanse (";
            line2 += bn::to_string<4>(_curse_charms);
            line2 += " charm)";
        }
        else
        {
            line2 = "Need charm to cleanse";
        }
        return;
    }

    if(node.type != UpgradeType::None)
    {
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
        case UpgradeType::Ability:
            line1 = "Ability tile";
            break;
        default:
            line1 = "Tile";
            break;
        }

        if(node.is_ability_slot)
        {
            line2 = "Ability equipped (A: swap, B: remove)";
        }
        else
        {
            line2 = "Placed (A: swap, B: remove)";
        }
        return;
    }

    // Empty slot
    line1 = node.is_ability_slot ? "Ability slot" : "Empty slot";

    if(!_graph.can_unlock(node.id))
    {
        line2 = "Locked path";
    }
    else if(!_has_any_tiles_in_inventory())
    {
        line2 = "Out of tiles";
    }
    else
    {
        line2 = "A: add tile, B: remove";
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

        // Text is UI, so it stays in screen space (no camera offset)
        _text_gen.generate(0, base_y,      line1, _text_sprites);
        _text_gen.generate(0, base_y + 10, line2, _text_sprites);

        bn::string<64> hint = "D-pad move, A: add/swap, B: remove";
        _text_gen.generate(0, base_y + 20, hint, _text_sprites);
    }
    else
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

        bn::string<64> line1;
        if(_mode == Mode::ChooseTileAdd)
        {
            line1 = "Add tile:";
        }
        else
        {
            line1 = "Swap tile:";
        }

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
        case UpgradeType::Ability:
            line2 = "Ability tile x";
            break;
        default:
            line2 = "Tile x";
            break;
        }

        line2 += bn::to_string<4>(stack.count);

        bn::string<64> line3 = "D-Pad select, A confirm, B back";

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
    const UpgradeNode& node = _graph.node(_selected_index);
    bool ability_slot = node.is_ability_slot;

    for(const TileStack& stack : _inventory)
    {
        if(stack.count <= 0)
        {
            continue;
        }

        if(ability_slot)
        {
            // Only ability tiles are valid in an ability slot
            if(stack.type == UpgradeType::Ability)
            {
                return true;
            }
        }
        else
        {
            // Non-ability slots should NOT show ability tiles
            if(stack.type != UpgradeType::Ability)
            {
                return true;
            }
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

    const UpgradeNode& node = _graph.node(_selected_index);
    bool ability_slot = node.is_ability_slot;

    int count = _inventory.size();
    for(int tries = 0; tries < count; ++tries)
    {
        _inventory_selected_index = (_inventory_selected_index + 1) % count;
        const TileStack& stack = _inventory[_inventory_selected_index];

        if(stack.count <= 0)
        {
            continue;
        }

        if(ability_slot)
        {
            if(stack.type == UpgradeType::Ability)
            {
                return;
            }
        }
        else
        {
            if(stack.type != UpgradeType::Ability)
            {
                return;
            }
        }
    }
}

void UpgradeScreen::_select_prev_inventory_tile()
{
    if(_inventory.empty())
    {
        return;
    }

    const UpgradeNode& node = _graph.node(_selected_index);
    bool ability_slot = node.is_ability_slot;

    int count = _inventory.size();
    for(int tries = 0; tries < count; ++tries)
    {
        _inventory_selected_index = (_inventory_selected_index - 1 + count) % count;
        const TileStack& stack = _inventory[_inventory_selected_index];

        if(stack.count <= 0)
        {
            continue;
        }

        if(ability_slot)
        {
            if(stack.type == UpgradeType::Ability)
            {
                return;
            }
        }
        else
        {
            if(stack.type != UpgradeType::Ability)
            {
                return;
            }
        }
    }
}

void UpgradeScreen::_add_tile_to_inventory(UpgradeType type)
{
    if(type == UpgradeType::None)
    {
        return;
    }

    for(TileStack& stack : _inventory)
    {
        if(stack.type == type)
        {
            stack.count++;
            return;
        }
    }

    if(_inventory.full())
    {
        return;
    }

    _inventory.push_back({ type, 1 });
}

bool UpgradeScreen::_current_slot_has_tile() const
{
    const UpgradeNode& node = _graph.node(_selected_index);
    return node.type != UpgradeType::None;
}

bool UpgradeScreen::_current_slot_can_accept_tile() const
{
    const UpgradeNode& node = _graph.node(_selected_index);

    if(node.is_cursed && !node.curse_cleared)
    {
        return false;
    }

    if(node.type != UpgradeType::None)
    {
        return false;
    }

    return _graph.can_unlock(node.id);
}

// ----------------------------------------------------------
// Curse handling
// ----------------------------------------------------------

void UpgradeScreen::_try_remove_curse()
{
    UpgradeNode& node = _graph.node(_selected_index);

    if(!node.is_cursed || node.curse_cleared || node.is_ability_slot)
    {
        return;
    }

    if(_curse_charms <= 0)
    {
        _update_text_panel();
        return;
    }

    _curse_charms--;
    node.curse_cleared = true;

    _graph.update_availability();
    _update_node_sprites();
    _update_text_panel();
}

// ----------------------------------------------------------
// Remove tile from slot
// ----------------------------------------------------------

void UpgradeScreen::_remove_current_tile()
{
    UpgradeNode& node = _graph.node(_selected_index);

    if(node.type == UpgradeType::None)
    {
        return;
    }

    UpgradeType removed = node.type;
    node.type = UpgradeType::None;

    _add_tile_to_inventory(removed);

    _graph.update_availability();
    _update_node_sprites();
    _update_text_panel();
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

    if(bn::keypad::b_pressed())
    {
        _remove_current_tile();
        return;
    }

    if(bn::keypad::a_pressed())
    {
        UpgradeNode& node = _graph.node(_selected_index);

        if(node.is_cursed && !node.curse_cleared && !node.is_ability_slot)
        {
            _try_remove_curse();
        }
        else if(node.type != UpgradeType::None)
        {
            _enter_choose_tile_swap_mode();
        }
        else
        {
            _enter_choose_tile_add_mode();
        }
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
        _mode = Mode::NavigateSlots;
        _update_text_panel();
        return;
    }

    if(bn::keypad::a_pressed())
    {
        if(_mode == Mode::ChooseTileAdd)
        {
            _place_selected_tile_add();
        }
        else
        {
            _place_selected_tile_swap();
        }
    }

    _update_text_panel();
}

void UpgradeScreen::_enter_choose_tile_add_mode()
{
    if(!_current_slot_can_accept_tile())
    {
        return;
    }

    if(!_has_any_tiles_in_inventory())
    {
        return;
    }

    const UpgradeNode& node = _graph.node(_selected_index);
    bool ability_slot = node.is_ability_slot;

    auto is_compatible = [&](const TileStack& stack)
    {
        if(stack.count <= 0)
        {
            return false;
        }
        if(ability_slot)
        {
            return stack.type == UpgradeType::Ability;
        }
        else
        {
            return stack.type != UpgradeType::Ability;
        }
    };

    if(!_inventory.empty() && !is_compatible(_inventory[_inventory_selected_index]))
    {
        _select_next_inventory_tile();
    }

    _mode = Mode::ChooseTileAdd;
}

void UpgradeScreen::_enter_choose_tile_swap_mode()
{
    if(!_current_slot_has_tile())
    {
        return;
    }

    if(!_has_any_tiles_in_inventory())
    {
        return;
    }

    const UpgradeNode& node = _graph.node(_selected_index);
    bool ability_slot = node.is_ability_slot;

    auto is_compatible = [&](const TileStack& stack)
    {
        if(stack.count <= 0)
        {
            return false;
        }
        if(ability_slot)
        {
            return stack.type == UpgradeType::Ability;
        }
        else
        {
            return stack.type != UpgradeType::Ability;
        }
    };

    if(!_inventory.empty() && !is_compatible(_inventory[_inventory_selected_index]))
    {
        _select_next_inventory_tile();
    }

    _mode = Mode::ChooseTileSwap;
}

void UpgradeScreen::_place_selected_tile_add()
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

    if(node.is_ability_slot && stack.type != UpgradeType::Ability)
    {
        return;
    }
    if(!node.is_ability_slot && stack.type == UpgradeType::Ability)
    {
        return;
    }

    node.type = stack.type;
    node.unlocked = true;
    stack.count--;

    _graph.update_availability();
    _update_node_sprites();

    _mode = Mode::NavigateSlots;
}

void UpgradeScreen::_place_selected_tile_swap()
{
    if(_inventory.empty() || !_has_any_tiles_in_inventory())
    {
        _mode = Mode::NavigateSlots;
        return;
    }

    if(!_current_slot_has_tile())
    {
        _mode = Mode::NavigateSlots;
        return;
    }

    TileStack& stack = _inventory[_inventory_selected_index];
    if(stack.count <= 0)
    {
        return;
    }

    UpgradeNode& node = _graph.node(_selected_index);

    if(node.is_ability_slot && stack.type != UpgradeType::Ability)
    {
        return;
    }
    if(!node.is_ability_slot && stack.type == UpgradeType::Ability)
    {
        return;
    }

    UpgradeType old_type = node.type;

    if(old_type == stack.type)
    {
        _mode = Mode::NavigateSlots;
        return;
    }

    stack.count--;
    _add_tile_to_inventory(old_type);

    node.type = stack.type;
    node.unlocked = true;

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
