#include "upgrade_graph.h"

#include "bn_array.h"

namespace
{
    UpgradeNode make_node(int id, UpgradeType type)
    {
        UpgradeNode n;
        n.id = id;
        n.grid_pos = bn::fixed_point(0, 0);
        n.type = type;
        n.unlocked = false;
        n.available = false;
        n.is_ability_slot = false;
        n.is_cursed = false;
        n.curse_cleared = false;
        return n;
    }

    void connect_two_way(UpgradeNode& a, NeighborDir dir_from_a_to_b,
                         UpgradeNode& b, NeighborDir dir_from_b_to_a)
    {
        a.set_neighbor(dir_from_a_to_b, b.id);
        b.set_neighbor(dir_from_b_to_a, a.id);
    }
}

UpgradeGraph UpgradeGraph::create_default()
{
    UpgradeGraph graph;
    graph._nodes.clear();

    // 32 nodes total.
    for(int i = 0; i < 32; ++i)
    {
        graph._nodes.push_back(make_node(i, UpgradeType::None));
    }

    // Convenience refs:
    UpgradeNode& n0  = graph._nodes[0];
    UpgradeNode& n1  = graph._nodes[1];
    UpgradeNode& n2  = graph._nodes[2];
    UpgradeNode& n3  = graph._nodes[3];
    UpgradeNode& n4  = graph._nodes[4];
    UpgradeNode& n5  = graph._nodes[5];
    UpgradeNode& n6  = graph._nodes[6];
    UpgradeNode& n7  = graph._nodes[7];
    UpgradeNode& n8  = graph._nodes[8];
    UpgradeNode& n9  = graph._nodes[9];
    UpgradeNode& n10 = graph._nodes[10];
    UpgradeNode& n11 = graph._nodes[11];
    UpgradeNode& n12 = graph._nodes[12];
    UpgradeNode& n13 = graph._nodes[13];
    UpgradeNode& n14 = graph._nodes[14];
    UpgradeNode& n15 = graph._nodes[15];
    UpgradeNode& n16 = graph._nodes[16];
    UpgradeNode& n17 = graph._nodes[17];
    UpgradeNode& n18 = graph._nodes[18];
    UpgradeNode& n19 = graph._nodes[19];
    UpgradeNode& n20 = graph._nodes[20];
    UpgradeNode& n21 = graph._nodes[21];
    UpgradeNode& n22 = graph._nodes[22];
    UpgradeNode& n23 = graph._nodes[23];

    UpgradeNode& n24 = graph._nodes[24]; // ability from n3
    UpgradeNode& n25 = graph._nodes[25]; // ability from n9
    UpgradeNode& n26 = graph._nodes[26]; // ability from n18
    UpgradeNode& n27 = graph._nodes[27]; // ability from n5

    UpgradeNode& n28 = graph._nodes[28]; // branch from n1
    UpgradeNode& n29 = graph._nodes[29]; // branch from n28
    UpgradeNode& n30 = graph._nodes[30]; // branch from n19
    UpgradeNode& n31 = graph._nodes[31]; // branch from n30

    // ---------------------------------------------------------------------
    // Neighbour connections: long snake path + several branches
    // ---------------------------------------------------------------------
    //
    // Main snake (0..23):
    //
    // Row y = -120:   n0 -> n1 -> n2 -> n3 -> n4 -> n5 -> n6 -> n7   (16px steps)
    // Column down:                      n7 -> n8 -> n9 -> n10        (16px steps)
    // Row y = -72:                      n10 -> n11 -> n12 -> n13 -> n14
    // Column down:                      n14 -> n15 -> n16 -> n17
    // Row y = -24:                      n17 -> n18 -> n19 -> n20
    // Column down:                      n20 -> n21 -> n22 -> n23

    // Top row (y = -120), moving right (16px apart)
    connect_two_way(n0, NeighborDir::East, n1, NeighborDir::West);
    connect_two_way(n1, NeighborDir::East, n2, NeighborDir::West);
    connect_two_way(n2, NeighborDir::East, n3, NeighborDir::West);
    connect_two_way(n3, NeighborDir::East, n4, NeighborDir::West);
    connect_two_way(n4, NeighborDir::East, n5, NeighborDir::West);
    connect_two_way(n5, NeighborDir::East, n6, NeighborDir::West);
    connect_two_way(n6, NeighborDir::East, n7, NeighborDir::West);

    // Column down from n7
    connect_two_way(n7,  NeighborDir::South, n8,  NeighborDir::North);
    connect_two_way(n8,  NeighborDir::South, n9,  NeighborDir::North);
    connect_two_way(n9,  NeighborDir::South, n10, NeighborDir::North);

    // Row at y = -72
    connect_two_way(n10, NeighborDir::East, n11, NeighborDir::West);
    connect_two_way(n11, NeighborDir::East, n12, NeighborDir::West);
    connect_two_way(n12, NeighborDir::East, n13, NeighborDir::West);
    connect_two_way(n13, NeighborDir::East, n14, NeighborDir::West);

    // Column down from n14
    connect_two_way(n14, NeighborDir::South, n15, NeighborDir::North);
    connect_two_way(n15, NeighborDir::South, n16, NeighborDir::North);
    connect_two_way(n16, NeighborDir::South, n17, NeighborDir::North);

    // Row at y = -24
    connect_two_way(n17, NeighborDir::West, n18, NeighborDir::East);
    connect_two_way(n18, NeighborDir::West, n19, NeighborDir::East);
    connect_two_way(n19, NeighborDir::West, n20, NeighborDir::East);

    // Column down from n20
    connect_two_way(n20, NeighborDir::South, n21, NeighborDir::North);
    connect_two_way(n21, NeighborDir::South, n22, NeighborDir::North);
    connect_two_way(n22, NeighborDir::South, n23, NeighborDir::North);

    // ---------------------------------------------------------------------
    // Branches with ability slots (24x24, 20px spacing to neighbours)
    // ---------------------------------------------------------------------

    // Ability from n3: go 20px up (north)
    connect_two_way(n3,  NeighborDir::North, n24, NeighborDir::South);
    // Ability from n5: go 20px up (north)
    connect_two_way(n5,  NeighborDir::North, n27, NeighborDir::South);
    // Ability from n9: go 20px left (west)
    connect_two_way(n9,  NeighborDir::West,  n25, NeighborDir::East);
    // Ability from n18: go 20px down (south)
    connect_two_way(n18, NeighborDir::South, n26, NeighborDir::North);

    // ---------------------------------------------------------------------
    // Extra normal branches (no ability)
    // ---------------------------------------------------------------------

    // Small vertical branch below n1:
    connect_two_way(n1,  NeighborDir::South, n28, NeighborDir::North);
    connect_two_way(n28, NeighborDir::South, n29, NeighborDir::North);

    // Small vertical branch above n19:
    connect_two_way(n19, NeighborDir::North, n30, NeighborDir::South);
    connect_two_way(n30, NeighborDir::North, n31, NeighborDir::South);

    connect_two_way(n12, NeighborDir::South, n31, NeighborDir::North);

    // ---------------------------------------------------------------------
    // Slot metadata
    // ---------------------------------------------------------------------

    // Ability slots:
    n24.is_ability_slot = true;
    n25.is_ability_slot = true;
    n26.is_ability_slot = true;
    n27.is_ability_slot = true;

    // Ability slots are never cursed:
    n24.is_cursed = false; n24.curse_cleared = false;
    n25.is_cursed = false; n25.curse_cleared = false;
    n26.is_cursed = false; n26.curse_cleared = false;
    n27.is_cursed = false; n27.curse_cleared = false;

    // Some cursed NORMAL tiles along the path/branches:
    n9.is_cursed   = true; n9.curse_cleared   = false;
    n21.is_cursed  = true; n21.curse_cleared  = false;
    n29.is_cursed  = true; n29.curse_cleared  = false;

    // ---------------------------------------------------------------------
    // Pixel positions
    //  - Normal neighbours: mostly 16px apart.
    //  - Ability neighbours: exactly 20px from their neighbour.
    // ---------------------------------------------------------------------

    // Main snake (0..23)
    n0.grid_pos  = bn::fixed_point(-160, -120);
    n1.grid_pos  = bn::fixed_point(-144, -120);
    n2.grid_pos  = bn::fixed_point(-128, -120);
    n3.grid_pos  = bn::fixed_point(-112, -120);
    n4.grid_pos  = bn::fixed_point( -96, -120);
    n5.grid_pos  = bn::fixed_point( -80, -120);
    n6.grid_pos  = bn::fixed_point( -64, -120);
    n7.grid_pos  = bn::fixed_point( -48, -120);

    n8.grid_pos  = bn::fixed_point( -48, -104);
    n9.grid_pos  = bn::fixed_point( -48,  -88);
    n10.grid_pos = bn::fixed_point( -48,  -72);

    n11.grid_pos = bn::fixed_point( -32,  -72);
    n12.grid_pos = bn::fixed_point( -16,  -72);
    n13.grid_pos = bn::fixed_point(   0,  -72);
    n14.grid_pos = bn::fixed_point(  16,  -72);

    n15.grid_pos = bn::fixed_point(  16,  -56);
    n16.grid_pos = bn::fixed_point(  16,  -40);
    n17.grid_pos = bn::fixed_point(  16,  -24);

    n18.grid_pos = bn::fixed_point(   0,  -24);
    n19.grid_pos = bn::fixed_point( -16,  -24);
    n20.grid_pos = bn::fixed_point( -32,  -24);

    n21.grid_pos = bn::fixed_point( -32,   -8);
    n22.grid_pos = bn::fixed_point( -32,    8);
    n23.grid_pos = bn::fixed_point( -32,   24);

    // Ability branches (24,25,26,27), 20px away from their neighbour:
    // n24: 20px above n3
    n24.grid_pos = bn::fixed_point(-112, -140);
    // n27: 20px above n5
    n27.grid_pos = bn::fixed_point( -80, -140);
    // n25: 20px left of n9
    n25.grid_pos = bn::fixed_point( -68,  -88);
    // n26: 20px below n18
    n26.grid_pos = bn::fixed_point(   0,   -4);

    // Normal branches:
    // Below n1:
    n28.grid_pos = bn::fixed_point(-144, -104);
    n29.grid_pos = bn::fixed_point(-144,  -88);

    // Above n19:
    n30.grid_pos = bn::fixed_point( -16,  -40);
    n31.grid_pos = bn::fixed_point( -16,  -56);

    // ---------------------------------------------------------------------
    // Compute availability based on unlocked + neighbours
    // ---------------------------------------------------------------------

    graph.update_availability();

    // Core starting node:
    n0.available = true;

    return graph;
}


int UpgradeGraph::_find_index_by_id(int id) const
{
    for(int i = 0, limit = _nodes.size(); i < limit; ++i)
    {
        if(_nodes[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

int UpgradeGraph::index_from_id(int id) const
{
    return _find_index_by_id(id);
}

bool UpgradeGraph::can_unlock(int node_id) const
{
    int index = _find_index_by_id(node_id);
    if(index < 0)
    {
        return false;
    }

    const UpgradeNode& node = _nodes[index];

    if (!node.available)
    {
        return false;
    }

    // Cursed slots can't be unlocked until cleansed
    if(node.is_cursed)
    {
        return node.curse_cleared;
    }

    return true;
}

void UpgradeGraph::unlock(int node_id)
{
    int index = _find_index_by_id(node_id);
    if(index < 0)
    {
        return;
    }

    UpgradeNode& node = _nodes[index];
    if(node.unlocked)
    {
        return;
    }

    node.unlocked = true;
    update_availability();
}

void UpgradeGraph::update_availability()
{
    for(UpgradeNode& node : _nodes)
    {
        node.available = false;
    }

    for(UpgradeNode& node : _nodes)
    {
        if(node.unlocked)
        {
            node.available = true;

            for(int d = 0; d < 4; ++d)
            {
                int neighbor_id = node.neighbors[d];
                if(neighbor_id < 0)
                {
                    continue;
                }

                int ni = _find_index_by_id(neighbor_id);
                if(ni >= 0)
                {
                    _nodes[ni].available = true;
                }
            }
        }
    }
}

// Unused now, but kept around in case you want auto-layout later.
void UpgradeGraph::assign_grid_positions(int start_id, bn::fixed_point start_pos)
{
    (void) start_id;
    (void) start_pos;
    // no-op with manual positions
}
