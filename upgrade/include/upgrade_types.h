#ifndef UPGRADE_TYPES_H
#define UPGRADE_TYPES_H

#include "bn_fixed_point.h"

// Order: 0 = North, 1 = East, 2 = South, 3 = West
enum class NeighborDir
{
    North = 0,
    East  = 1,
    South = 2,
    West  = 3
};

enum class UpgradeType
{
    None,
    HpUp,
    AttackUp,
    DefenseUp,
    Ability,        // active ability tile
};

struct UpgradeNode
{
    int id = -1;

    // Here we treat grid_pos as SCREEN PIXELS (x, y),
    // not "grid units". You hardcode these in create_default.
    bn::fixed_point grid_pos;

    // The tile currently placed in this slot (None = empty)
    UpgradeType type = UpgradeType::None;

    // Path / progression flags:
    bool unlocked  = false;   // this node has been activated (tile placed here)
    bool available = false;   // reachable by path logic (neighbors of unlocked nodes)
    bool root      = false;   // start of a path

    // Slot metadata:
    bool is_ability_slot = false;   // only accepts Ability tiles
    bool is_cursed       = false;   // starts cursed
    bool curse_cleared   = false;   // true once curse is removed using an item

    // Neighbor IDs: North, East, South, West
    int neighbors[4] = { -1, -1, -1, -1 };

    int neighbor_id(NeighborDir dir) const
    {
        return neighbors[static_cast<int>(dir)];
    }

    void set_neighbor(NeighborDir dir, int node_id)
    {
        neighbors[static_cast<int>(dir)] = node_id;
    }
};

#endif // UPGRADE_TYPES_H
