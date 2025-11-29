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
    SpeedUp,
    AbilitySlot
};

struct UpgradeNode
{
    int id = -1;

    // Logical layout position (for drawing & debugging)
    bn::fixed_point grid_pos;

    UpgradeType type = UpgradeType::None;

    bool unlocked  = false;
    bool available = false;

    // Neighbors by direction: store node IDs, or -1 if none
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
