#include "health_bar.h"

#include "bn_sprite_items_health_bar.h"
#include "bn_math.h"

void HealthBar::attach_camera(const bn::camera_ptr& camera) {
    if(_sprite)
    {
        _sprite->set_camera(camera);
    }
    _camera = camera;
}

void HealthBar::_ensure_created()
{
    if(!_sprite)
    {
        _sprite = bn::sprite_items::health_bar.create_sprite(0, 0, 0);
        _sprite->set_visible(false);
        _sprite->set_bg_priority(0);
        _sprite->set_camera(_camera.value());
    }
}

void HealthBar::hide()
{
    if(_sprite)
    {
        _sprite->set_visible(false);
    }
}

void HealthBar::update(
    const bn::fixed_point& entity_pos,
    int health,
    int max_health,
    int z_order)
{
    if(max_health <= 0)
    {
        hide();
        return;
    }

    _ensure_created();

    // Clamp health between 0 and max_health
    if(health < 0)
    {
        health = 0;
    }
    else if(health > max_health)
    {
        health = max_health;
    }

    // Map [0, max_health] -> [0, STAGES-1] with rounding
    // stage = round(health / max_health * (STAGES-1))
    int stage = 0;

    if(health == 0)
    {
        stage = 0;  // explicitly empty bar
    }
    else
    {
        const int numerator   = health * (STAGES - 1) + max_health / 2;
        stage = numerator / max_health;   // integer division with rounding
    }

    if(stage < 0)
    {
        stage = 0;
    }
    else if(stage >= STAGES)
    {
        stage = STAGES - 1;
    }

    // Position above the entity
    const bn::fixed_point bar_pos(
        entity_pos.x(),
        entity_pos.y() + bn::fixed(_y_offset)
    );

    _sprite->set_position(bar_pos);
    _sprite->set_visible(true);

    // Draw slightly in front of the entity
    _sprite->set_z_order(z_order - 1);

    // Select the correct frame from the spritesheet (0..14)
    // Assumes the sprite item is defined with 15 graphics frames.
    _sprite->set_tiles(bn::sprite_items::health_bar.tiles_item(), stage);
}
