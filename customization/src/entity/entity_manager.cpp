#include "entity_manager.h"
#include "hitbox.h"

EntityManager::EntityManager(Player* player, RoomId room) :
    _player(player), _current_room(room)
{
}

EntityManager::RoomEnemies* EntityManager::_find_room_bucket(RoomId room)
{
    for(int i = 0; i < _rooms.size(); ++i)
    {
        if(_rooms[i].room == room)
        {
            return &_rooms[i];
        }
    }
    return nullptr;
}

EntityManager::RoomEnemies& EntityManager::_ensure_room_bucket(RoomId room)
{
    if(RoomEnemies* existing = _find_room_bucket(room))
    {
        return *existing;
    }

    BN_ASSERT(_rooms.size() < max_rooms, "Too many rooms in EntityManager");

    RoomEnemies bucket;
    bucket.room = room;
    bucket.enemies.clear();

    _rooms.push_back(bucket);
    return _rooms.back();
}

void EntityManager::set_current_room(RoomId room)
{
    if(_current_room == room)
    {
        return;
    }

    RoomEnemies& cur_bucket = _ensure_room_bucket(_current_room);
    cur_bucket.enemies = _enemies;   // save pointer list for that room

    for(Enemy* enemy : cur_bucket.enemies)
    {
        if(enemy)
        {
            enemy->set_active(false);
        }
    }

    // 2) Switch current room id
    _current_room = room;

    // 3) Load enemies for the new room
    RoomEnemies& new_bucket = _ensure_room_bucket(room);
    _enemies = new_bucket.enemies;

    // 4) Activate enemies in the new room so they appear and run AI
    for(Enemy* enemy : _enemies)
    {
        if(enemy)
        {
            enemy->set_active(true);
        }
    }
}

void EntityManager::add_enemy(Enemy* enemy, RoomId room)
{
    if(!enemy)
    {
        return;
    }

    // Get or create the bucket for the specific room
    RoomEnemies& bucket = _ensure_room_bucket(room);

    // Enforce room-local capacity
    if(bucket.enemies.size() >= max_enemies)
    {
        return;
    }

    // Add to that room's persistent list
    bucket.enemies.push_back(enemy);

    // If this is also the *current* room, mirror into the active list
    if(_current_room == room)
    {
        if(_enemies.size() < max_enemies)
        {
            _enemies.push_back(enemy);
            enemy->set_active(true);
        }
        else
        {
            enemy->set_active(false);
        }
    }
    else
    {
        // Not the current room; keep it inactive for now
        enemy->set_active(false);
    }
}

void EntityManager::clear_enemies()
{
    // Clear only the current room's enemies
    RoomEnemies& bucket = _ensure_room_bucket(_current_room);
    bucket.enemies.clear();

    _enemies.clear();
}

void EntityManager::update()
{
    _update_all();
    _handle_player_attacks_enemies();
    _handle_enemy_attacks_player();
    _handle_bumps();
}

void EntityManager::_update_all()
{
    if(_player)
    {
        _player->update();
    }

    for(Enemy* enemy : _enemies)
    {
        if(enemy)
        {
            enemy->update();
        }
    }
}

template<typename Func>
void EntityManager::_for_each_alive_enemy(Func&& func)
{
    for(Enemy* enemy : _enemies)
    {
        if(!enemy || !enemy->is_alive())
        {
            continue;
        }
        func(enemy);
    }
}

void EntityManager::_handle_player_attacks_enemies()
{
    if(!_player || !_player->is_alive() || !_player->is_attacking())
    {
        return;
    }

    _for_each_alive_enemy([&](Enemy* enemy)
    {
        if(_player->attack_hits(*enemy))
        {
            enemy->take_damage(_player->damage(), _player->position());
        }
    });
}

void EntityManager::_handle_enemy_attacks_player()
{
    if(!_player || !_player->is_alive())
    {
        return;
    }

    _for_each_alive_enemy([&](Enemy* enemy)
    {
        if(enemy->is_attacking() && enemy->attack_hits(*_player))
        {
            _player->take_damage(enemy->damage(), enemy->position());
        }
    });
}

void EntityManager::_handle_bumps()
{
    // Player vs enemies
    if(_player && _player->is_alive())
    {
        _for_each_alive_enemy([&](Enemy* enemy)
        {
            if(_player->overlaps(*enemy))
            {
                _separate_pair(_player, enemy);
            }
        });
    }

    // Enemy vs enemy separation
    const int enemy_count = _enemies.size();
    for(int i = 0; i < enemy_count; ++i)
    {
        Enemy* a = _enemies[i];
        if(!a || !a->is_alive())
        {
            continue;
        }

        for(int j = i + 1; j < enemy_count; ++j)
        {
            Enemy* b = _enemies[j];
            if(!b || !b->is_alive())
            {
                continue;
            }

            if(a->overlaps(*b))
            {
                _separate_pair(a, b);
            }
        }
    }
}

// Minimal AABB penetration resolution using hurt boxes
void EntityManager::_separate_pair(Entity* a, Entity* b)
{
    if(!a || !b)
    {
        return;
    }
    const Hitbox& ha = a->hurt_box();
    const Hitbox& hb = b->hurt_box();

    const bn::fixed_point pa = a->position();
    const bn::fixed_point pb = b->position();

    const bn::fixed_point ca = ha.center(pa);
    const bn::fixed_point cb = hb.center(pb);

    const bn::fixed dx = ca.x() - cb.x();
    const bn::fixed dy = ca.y() - cb.y();

    // How much they overlap on each axis
    const bn::fixed overlap_x = ha.half_width  + hb.half_width  - bn::abs(dx);
    const bn::fixed overlap_y = ha.half_height + hb.half_height - bn::abs(dy);

    // No actual overlap (just touching or separated)
    if(overlap_x <= 0 || overlap_y <= 0)
    {
        return;
    }

    // Helper to move them symmetrically
    auto separate = [&](const bn::fixed_point& delta)
    {
        a->move_by(delta);
        b->move_by(-delta);
    };

    // Resolve along the "cheapest" axis
    if(overlap_x < overlap_y)
    {
        const bn::fixed push = overlap_x / 2;
        const bn::fixed dir  = (dx > 0) ? bn::fixed(1) : bn::fixed(-1);

        separate(bn::fixed_point(push * dir, 0));
    }
    else
    {
        const bn::fixed push = overlap_y / 2;
        const bn::fixed dir  = (dy > 0) ? bn::fixed(1) : bn::fixed(-1);

        separate(bn::fixed_point(0, push * dir));
    }
}
