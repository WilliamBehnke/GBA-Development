#ifndef UPGRADE_GRAPH_H
#define UPGRADE_GRAPH_H

#include "bn_vector.h"
#include "upgrade_types.h"

class UpgradeGraph
{
public:
    static constexpr int max_nodes = 16;

    UpgradeGraph() = default;

    static UpgradeGraph create_default();

    int node_count() const { return _nodes.size(); }

    UpgradeNode& node(int index) { return _nodes[index]; }
    const UpgradeNode& node(int index) const { return _nodes[index]; }

    const bn::vector<UpgradeNode, max_nodes>& nodes() const { return _nodes; }

    bool can_unlock(int node_id) const;
    void unlock(int node_id);
    void update_availability();

    void assign_grid_positions(int start_id, bn::fixed_point start_pos);

private:
    bn::vector<UpgradeNode, max_nodes> _nodes;

    int _find_index_by_id(int id) const;
};

#endif // UPGRADE_GRAPH_H
