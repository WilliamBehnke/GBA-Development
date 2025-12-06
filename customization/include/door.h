#ifndef DOOR_H
#define DOOR_H

#include "bn_fixed_point.h"
#include "bn_fixed.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"

#include "world_map_data.h"
#include "bn_sprite_items_door.h"

struct DoorTarget
{
    RoomId          room_id;
    bn::fixed_point spawn_pos;
};

class Door
{
public:
    Door(const bn::fixed_point& center,
         const bn::fixed& half_width,
         const bn::fixed& half_height,
         RoomId target_room,
         const bn::fixed_point& target_spawn_pos);

    void set_camera(const bn::optional<bn::camera_ptr>& camera);
    void update();

    DoorTarget target() const { return { _target_room, _target_spawn_pos }; }

    // Check if a world position (like the player's feet) is inside this door's area
    bool contains(const bn::fixed_point& world_pos) const;

private:
    bn::fixed_point _center;
    bn::fixed       _half_width;   // in pixels (e.g. 16px => 4 tiles wide)
    bn::fixed       _half_height;  // in pixels

    RoomId          _target_room;
    bn::fixed_point _target_spawn_pos;

    bn::sprite_ptr  _sprite;
};

#endif // DOOR_H
