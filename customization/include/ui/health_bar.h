#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"
#include "bn_camera_ptr.h"

class HealthBar
{
public:
    HealthBar() = default;

    void update(
        const bn::fixed_point& entity_pos,
        int health,
        int max_health,
        int z_order
    );

    void attach_camera(const bn::camera_ptr& camera);

    // Optional: force-hide without touching health.
    void hide();

private:
    void _ensure_created();

    bn::optional<bn::sprite_ptr> _sprite;

    bn::optional<bn::camera_ptr> _camera;

    int _y_offset = -14;

    static constexpr int STAGES = 15;  // 0..14 (0 = empty, 14 = full)
};

#endif // HEALTH_BAR_H
