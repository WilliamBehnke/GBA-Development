// EntityManager.cpp
#include "EntityManager.h"
#include "Hitbox.h"   // for Hitbox::center etc.

EntityManager::EntityManager(Entity* player,
                             int player_attack_damage,
                             int enemy_attack_damage,
                             int enemy_bump_damage) :
    _player(player),
    _player_attack_damage(player_attack_damage),
    _enemy_attack_damage(enemy_attack_damage),
    _enemy_bump_damage(enemy_bump_damage)
{
}

void EntityManager::add_enemy(Entity* enemy)
{
    if(enemy && _enemies.size() < max_enemies)
    {
        _enemies.push_back(enemy);
    }
}

void EntityManager::clear_enemies()
{
    _enemies.clear();
}

void EntityManager::update()
{
    _update_all();
    _handle_player_attacks_enemies();
    _handle_enemy_attacks_player();
    _handle_bumps();
}

// ------------------------------------------------------
// Internal helpers
// ------------------------------------------------------

void EntityManager::_update_all()
{
    if(_player)
    {
        _player->update();
    }

    for(Entity* enemy : _enemies)
    {
        if(enemy)
        {
            enemy->update();
        }
    }
}

// Player melee hits enemies
void EntityManager::_handle_player_attacks_enemies()
{
    if(!_player || !_player->is_alive())
        return;

    if(!_player->is_attacking())
        return;

    for(Entity* enemy : _enemies)
    {
        if(!enemy || !enemy->is_alive())
            continue;

        if(_player->attack_hits(*enemy))
        {
            enemy->take_damage(_player_attack_damage, _player->position());
        }
    }
}

// Enemies attacking player (e.g. melee swing)
void EntityManager::_handle_enemy_attacks_player()
{
    if(!_player || !_player->is_alive())
        return;

    for(Entity* enemy : _enemies)
    {
        if(!enemy || !enemy->is_alive())
            continue;

        if(!enemy->is_attacking())
            continue;

        if(enemy->attack_hits(*_player))
        {
            _player->take_damage(_enemy_attack_damage, enemy->position());
        }
    }
}

// Push overlapping entities apart (player <-> enemies, and enemy <-> enemy if you want)
void EntityManager::_handle_bumps()
{
    if(_player && _player->is_alive())
    {
        for(Entity* enemy : _enemies)
        {
            if(!enemy || !enemy->is_alive())
                continue;

            if(_player->overlaps(*enemy))
            {
                _separate_pair(*_player, *enemy);

                // Optional contact damage on bump:
                if(_enemy_bump_damage > 0)
                {
                    _player->take_damage(_enemy_bump_damage, enemy->position());
                }
            }
        }
    }

    // OPTIONAL: enemy-enemy separation
    for(int i = 0; i < _enemies.size(); ++i)
    {
        Entity* a = _enemies[i];
        if(!a || !a->is_alive())
            continue;

        for(int j = i + 1; j < _enemies.size(); ++j)
        {
            Entity* b = _enemies[j];
            if(!b || !b->is_alive())
                continue;

            if(a->overlaps(*b))
            {
                _separate_pair(*a, *b);
            }
        }
    }
}

// Minimal AABB penetration resolution using your Hitbox data
void EntityManager::_separate_pair(Entity& a, Entity& b)
{
    const Hitbox& ha = a.hurt_box();
    const Hitbox& hb = b.hurt_box();

    bn::fixed_point pa = a.position();
    bn::fixed_point pb = b.position();

    bn::fixed_point ca = ha.center(pa);
    bn::fixed_point cb = hb.center(pb);

    bn::fixed dx = ca.x() - cb.x();
    bn::fixed dy = ca.y() - cb.y();

    // How much they overlap in each axis
    bn::fixed overlap_x = ha.half_width + hb.half_width - bn::abs(dx);
    bn::fixed overlap_y = ha.half_height + hb.half_height - bn::abs(dy);

    if(overlap_x <= 0 || overlap_y <= 0)
        return; // no actual overlap, just touching or separate

    // Resolve along the "cheapest" axis
    if(overlap_x < overlap_y)
    {
        bn::fixed push = overlap_x / 2;
        if(dx > 0)
        {
            // a is to the right of b, push a right, b left
            a.move_by(bn::fixed_point(push, 0));
            b.move_by(bn::fixed_point(-push, 0));
        }
        else
        {
            a.move_by(bn::fixed_point(-push, 0));
            b.move_by(bn::fixed_point(push, 0));
        }
    }
    else
    {
        bn::fixed push = overlap_y / 2;
        if(dy > 0)
        {
            // a is below b, push a down, b up
            a.move_by(bn::fixed_point(0, push));
            b.move_by(bn::fixed_point(0, -push));
        }
        else
        {
            a.move_by(bn::fixed_point(0, -push));
            b.move_by(bn::fixed_point(0, push));
        }
    }
}
