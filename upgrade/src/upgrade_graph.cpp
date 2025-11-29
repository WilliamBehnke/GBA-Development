#include "upgrade_graph.h"

#include "bn_array.h"

namespace
{
    UpgradeNode make_node(int id, bn::fixed_point grid_pos, UpgradeType type)
    {
        UpgradeNode n;
        n.id = id;
        n.grid_pos = grid_pos;
        n.type = type;
        n.unlocked = false;
        n.available = false;
        // neighbors[] is already initialized to {-1, -1, -1, -1} by default
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

    // We don't care about grid_pos here; it'll be overwritten by assign_grid_positions.
    auto zero_pos = bn::fixed_point(0, 0);

    // IDs 0..8:
    graph._nodes.push_back(make_node(0, zero_pos, UpgradeType::None));  // core
    graph._nodes.push_back(make_node(1, zero_pos, UpgradeType::None));  // down from 0
    graph._nodes.push_back(make_node(2, zero_pos, UpgradeType::None));  // down from 1
    graph._nodes.push_back(make_node(3, zero_pos, UpgradeType::None));  // down from 2
    graph._nodes.push_back(make_node(4, zero_pos, UpgradeType::None));  // down from 3
    graph._nodes.push_back(make_node(5, zero_pos, UpgradeType::None));  // down from 4

    graph._nodes.push_back(make_node(6, zero_pos, UpgradeType::None));  // branch right from 2
    graph._nodes.push_back(make_node(7, zero_pos, UpgradeType::None));  // right from 6
    graph._nodes.push_back(make_node(8, zero_pos, UpgradeType::None));  // down from 7
    graph._nodes.push_back(make_node(9, zero_pos, UpgradeType::None));  // branch left/deep from 4/5 area

    // Convenience refs:
    UpgradeNode& n0 = graph._nodes[0];
    UpgradeNode& n1 = graph._nodes[1];
    UpgradeNode& n2 = graph._nodes[2];
    UpgradeNode& n3 = graph._nodes[3];
    UpgradeNode& n4 = graph._nodes[4];
    UpgradeNode& n5 = graph._nodes[5];
    UpgradeNode& n6 = graph._nodes[6];
    UpgradeNode& n7 = graph._nodes[7];
    UpgradeNode& n8 = graph._nodes[8];
    UpgradeNode& n9 = graph._nodes[9];

    // Backbone vertical chain: 0 <-> 1 <-> 2 <-> 3 <-> 4 <-> 5
    connect_two_way(n0, NeighborDir::South, n1, NeighborDir::North);
    connect_two_way(n1, NeighborDir::South, n2, NeighborDir::North);
    connect_two_way(n2, NeighborDir::South, n3, NeighborDir::North);
    connect_two_way(n3, NeighborDir::South, n4, NeighborDir::North);
    connect_two_way(n4, NeighborDir::South, n5, NeighborDir::North);

    // Branch A: from 2 to the right, then down.
    connect_two_way(n2, NeighborDir::East,  n6, NeighborDir::West);
    connect_two_way(n6, NeighborDir::East,  n7, NeighborDir::West);
    connect_two_way(n7, NeighborDir::South, n8, NeighborDir::North);

    // Branch B: from 4 down to 8, then across to 5
    connect_two_way(n9, NeighborDir::East,  n5, NeighborDir::West);

    // 🔹 Now assign positions starting at node 0, at (0,-1) in grid space:
    graph.assign_grid_positions(0, bn::fixed_point(0, -1));

    // Starting node unlocked:
    n1.available = true;

    return graph;
}

void UpgradeGraph::assign_grid_positions(int start_id, bn::fixed_point start_pos)
{
    // Simple BFS over the graph, stepping 1 grid unit per neighbor direction.

    bool visited[max_nodes];
    for(int i = 0; i < max_nodes; ++i)
    {
        visited[i] = false;
    }

    int start_index = _find_index_by_id(start_id);
    if(start_index < 0)
    {
        return;
    }

    _nodes[start_index].grid_pos = start_pos;
    visited[start_index] = true;

    bn::vector<int, max_nodes> queue;
    queue.push_back(start_index);

    for(int qi = 0; qi < queue.size(); ++qi)
    {
        int idx = queue[qi];
        const UpgradeNode& node = _nodes[idx];

        for(int d = 0; d < 4; ++d)
        {
            int neighbor_id = node.neighbors[d];
            if(neighbor_id < 0)
            {
                continue;
            }

            int ni = _find_index_by_id(neighbor_id);
            if(ni < 0 || visited[ni])
            {
                continue;
            }

            bn::fixed x = node.grid_pos.x();
            bn::fixed y = node.grid_pos.y();

            switch(static_cast<NeighborDir>(d))
            {
            case NeighborDir::North: y -= 1; break;
            case NeighborDir::South: y += 1; break;
            case NeighborDir::East:  x += 1; break;
            case NeighborDir::West:  x -= 1; break;
            }

            _nodes[ni].grid_pos = bn::fixed_point(x, y);
            visited[ni] = true;
            queue.push_back(ni);
        }
    }
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

bool UpgradeGraph::can_unlock(int node_id) const
{
    int index = _find_index_by_id(node_id);
    if(index < 0)
    {
        return false;
    }

    return _nodes[index].available;
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
    // Reset
    for(UpgradeNode& node : _nodes)
    {
        node.available = false;
    }

    // Unlocked are always available
    for(UpgradeNode& node : _nodes)
    {
        if(node.unlocked)
        {
            node.available = true;

            // Their directional neighbors become available too
            for(int d = 0; d < 4; ++d)
            {
                int neighbor_id = node.neighbors[d];
                if(neighbor_id < 0)
                {
                    continue;
                }

                int ni = _find_index_by_id(neighbor_id);
                if(ni >= 0 && !_nodes[ni].unlocked)
                {
                    _nodes[ni].available = true;
                }
            }
        }
    }
}
