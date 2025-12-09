#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "bn_vector.h"

class EntityManager
{
public:
    static constexpr int max_enemies = 32;

    EntityManager(Player* player = nullptr);

    void set_player(Player* player) { _player = player; }
    Player* player() const { return _player; }

    void add_enemy(Enemy* enemy);
    void clear_enemies();

    bn::vector<Enemy*, max_enemies>& enemies() { return _enemies; }
    const bn::vector<Enemy*, max_enemies>& enemies() const { return _enemies; }

    // Per-frame update
    void update();

private:
    Player* _player = nullptr;
    bn::vector<Enemy*, max_enemies> _enemies;

    // Internal update phases
    void _update_all();
    void _handle_player_attacks_enemies();
    void _handle_enemy_attacks_player();
    void _handle_bumps();

    // Collision resolution
    void _separate_pair(Entity* a, Entity* b);

    // Utility iteration helper for all alive enemies
    template<typename Func>
    void _for_each_alive_enemy(Func&& func);
};

#endif // ENTITY_MANAGER_H
