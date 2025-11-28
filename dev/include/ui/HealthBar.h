#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"

class HealthBar {
public:
    HealthBar() = default;

    void update(
        const bn::fixed_point& entity_pos,
        int health,
        int max_health,
        int z_order
    );

    // Optional: force-hide without touching health.
    void hide();

private:
    void _ensure_created();

    bn::optional<bn::sprite_ptr> _sprite;

    int _y_offset = -19;

    static constexpr int STAGES = 16;  // 0..15 (0 = empty, 15 = full)
};

#endif // HEALTH_BAR_H
