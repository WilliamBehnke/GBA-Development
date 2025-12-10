#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "world_map_data.h"
#include "bn_vector.h"

class EntityManager
{
public:
    static constexpr int max_enemies = 32;
    static constexpr int max_rooms   = ROOM_COUNT;

    EntityManager(Player* player = nullptr, RoomId room = RoomId::MainRoom);

    void set_player(Player* player) { _player = player; }
    Player* player() const { return _player; }

    // Tell the manager which room is currently active.
    // Call this whenever the player enters a new room.
    void set_current_room(RoomId room);
    RoomId current_room() const { return _current_room; }

    void add_enemy(Enemy* enemy)
    {
        add_enemy(enemy, _current_room);
    }
    void add_enemy(Enemy* enemy, RoomId room);

    // Clears enemies only in the current room.
    void clear_enemies();

    bn::vector<Enemy*, max_enemies>& enemies() { return _enemies; }
    const bn::vector<Enemy*, max_enemies>& enemies() const { return _enemies; }

    // Per-frame update
    void update();

private:
    Player* _player = nullptr;

    // Enemies in the current active room (for fast iteration)
    bn::vector<Enemy*, max_enemies> _enemies;

    // Per-room enemy lists
    struct RoomEnemies
    {
        RoomId room;
        bn::vector<Enemy*, max_enemies> enemies;
    };

    bn::vector<RoomEnemies, max_rooms> _rooms;
    RoomId _current_room;

    // Look up or create the bucket for a room
    RoomEnemies* _find_room_bucket(RoomId room);
    RoomEnemies& _ensure_room_bucket(RoomId room);

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
