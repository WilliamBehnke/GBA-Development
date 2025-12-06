#include "door.h"
#include "bn_math.h"

Door::Door(const bn::fixed_point& center,
           const bn::fixed& half_width,
           const bn::fixed& half_height,
           RoomId target_room,
           const bn::fixed_point& target_spawn_pos) :
    _center(center),
    _half_width(half_width),
    _half_height(half_height),
    _target_room(target_room),
    _target_spawn_pos(target_spawn_pos),
    _sprite(bn::sprite_items::door.create_sprite(center.x(), center.y()))
{
    _sprite.set_bg_priority(1);
    _sprite.set_z_order(10);
}

void Door::set_camera(const bn::optional<bn::camera_ptr>& camera)
{
    if(camera)
        _sprite.set_camera(camera.value());
    else
        _sprite.remove_camera();
}

void Door::update()
{
}

bool Door::contains(const bn::fixed_point& world_pos) const
{
    const bn::fixed dx = world_pos.x() - _center.x();
    const bn::fixed dy = world_pos.y() - _center.y();

    return (bn::abs(dx) <= _half_width) &&
           (bn::abs(dy) <= _half_height);
}
