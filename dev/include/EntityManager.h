#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "bn_vector.h"
#include "Entity.h"

class EntityManager
{
public:
    static constexpr int max_enemies = 32;

    EntityManager(Entity* player = nullptr,
                  int player_attack_damage = 1,
                  int enemy_attack_damage  = 1,
                  int enemy_bump_damage    = 0);

    // ------------------------------------------------------
    // Setup
    // ------------------------------------------------------
    void set_player(Entity* player) { _player = player; }

    Entity* player() const { return _player; }

    void add_enemy(Entity* enemy);
    void clear_enemies();

    bn::vector<Entity*, max_enemies>& enemies() { return _enemies; }
    const bn::vector<Entity*, max_enemies>& enemies() const { return _enemies; }

    // ------------------------------------------------------
    // Per-frame update
    // ------------------------------------------------------
    void update();

private:
    Entity* _player = nullptr;
    bn::vector<Entity*, max_enemies> _enemies;

    int _player_attack_damage;
    int _enemy_attack_damage;
    int _enemy_bump_damage;  // optional contact damage on bump

    void _update_all();
    void _handle_player_attacks_enemies();
    void _handle_enemy_attacks_player();
    void _handle_bumps();

    void _separate_pair(Entity& a, Entity& b);
};

#endif // ENTITY_MANAGER_H
