#ifndef UPGRADE_GRAPH_H
#define UPGRADE_GRAPH_H

#include "bn_vector.h"
#include "bn_fixed_point.h"

#include "upgrade_types.h"

class UpgradeGraph
{
public:
    static constexpr int max_nodes = 32;

    UpgradeGraph() = default;

    static UpgradeGraph create_default();

    int node_count() const
    {
        return _nodes.size();
    }

    UpgradeNode& node(int index)
    {
        return _nodes[index];
    }

    const UpgradeNode& node(int index) const
    {
        return _nodes[index];
    }

    const bn::vector<UpgradeNode, max_nodes>& nodes() const
    {
        return _nodes;
    }

    bool can_unlock(int node_id) const;
    void unlock(int node_id);
    void update_availability();

    int index_from_id(int id) const;

    // This still exists but is no longer used now that positions are hardcoded.
    void assign_grid_positions(int start_id, bn::fixed_point start_pos);

private:
    bn::vector<UpgradeNode, max_nodes> _nodes;

    int _find_index_by_id(int id) const;

    friend class UpgradeScreen;
};

#endif // UPGRADE_GRAPH_H
