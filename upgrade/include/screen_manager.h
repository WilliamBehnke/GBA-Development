#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "upgrade_graph.h"
#include "upgrade_screen.h"

#include "bn_sprite_text_generator.h"
#include "common_fixed_8x8_sprite_font.h"

enum class ScreenType
{
    Upgrade,
    // Future: Title, Overworld, Battle, etc.
};

class ScreenManager
{
public:
    ScreenManager();

    void update();

private:
    ScreenType _current_type = ScreenType::Upgrade;

    UpgradeGraph _graph;
    bn::sprite_text_generator _text_gen;
    UpgradeScreen _upgrade_screen;
};

#endif // SCREEN_MANAGER_H
