#include "Entity.h"

Entity::Entity(BaseSprite* sprite,
               int max_health,
               int damage,
               const Hitbox& hurt_box,
               const Hitbox& attack_box,
               int invuln_duration_frames) :
    _sprite(sprite),
    _health(max_health),
    _max_health(max_health),
    _damage(damage),
    _hurt_box(hurt_box),
    _attack_box(attack_box),
    _invuln_timer(0),
    _invuln_duration(invuln_duration_frames),
    _knockback_dir(0, 0),
    _knockback_timer(0),
    _knockback_duration(8),
    _knockback_strength(1)
{}

void Entity::update() {
    _apply_knockback();

    if(_sprite) {
        _sprite->update();

        // Y-sort: bigger Y = closer to camera (on top)
        bn::fixed_point pos = _sprite->sprite().position();
        _sprite->sprite().set_z_order(-pos.y().integer());
    }

    _tick_invulnerability();
}

bn::fixed_point Entity::position() const {
    return _sprite ? _sprite->sprite().position() : bn::fixed_point();
}

void Entity::move_by(const bn::fixed_point& delta) {
    if(_sprite) {
        bn::fixed_point pos = _sprite->sprite().position();
        _sprite->sprite().set_position(pos + delta);
    }
}

bool Entity::is_attacking() const {
    return _sprite ? _sprite->is_attacking() : false;
}

void Entity::take_damage(int amount) {
    // Fallback: no direction info (no meaningful knockback dir)
    take_damage(amount, position());
}

void Entity::take_damage(int amount, const bn::fixed_point& source_pos) {
    if(amount <= 0 || !is_alive() || is_invulnerable()) {
        return;
    }

    _health -= amount;
    if(_health < 0) {
        _health = 0;
    }

    _invuln_timer = _invuln_duration;

    if(_sprite) {
        if(_health <= 0) {
            _sprite->play_death();
        } else {
            _sprite->play_hurt();
        }
    }

    _start_knockback(source_pos);

    DamageNumbers::spawn(position(), amount);
}

bool Entity::overlaps(const Entity& other) const {
    return hitboxes_intersect(
        _hurt_box,  position(),
        other._hurt_box, other.position()
    );
}

bool Entity::attack_hits(const Entity& other) const {
    return hitboxes_intersect(
        _attack_box, position(),
        other._hurt_box, other.position()
    );
}

void Entity::_tick_invulnerability() {
    if(_invuln_timer > 0) {
        --_invuln_timer;
    }
}

void Entity::_start_knockback(const bn::fixed_point& source_pos) {
    if(!_sprite) {
        return;
    }

    bn::fixed_point pos = _sprite->sprite().position();
    bn::fixed dx = pos.x() - source_pos.x();
    bn::fixed dy = pos.y() - source_pos.y();

    bn::fixed len_sq = dx * dx + dy * dy;
    if(len_sq == 0) {
        _knockback_dir = bn::fixed_point(0, -1);
    } else {
        bn::fixed len = bn::sqrt(len_sq);
        _knockback_dir = bn::fixed_point(dx / len, dy / len);
    }

    _knockback_timer = _knockback_duration;
}

void Entity::_apply_knockback() {
    if(_knockback_timer <= 0 || !_sprite) {
        return;
    }

    --_knockback_timer;

    bn::fixed_point pos = _sprite->sprite().position();
    pos.set_x(pos.x() + _knockback_dir.x() * _knockback_strength);
    pos.set_y(pos.y() + _knockback_dir.y() * _knockback_strength);
    _sprite->sprite().set_position(pos);
}
