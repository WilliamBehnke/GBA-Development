#include "screen_manager.h"

#include "bn_core.h"

ScreenManager::ScreenManager() :
    _graph(UpgradeGraph::create_default()),
    _text_gen(common::fixed_8x8_sprite_font),
    _upgrade_screen(_graph, _text_gen)
{
    _text_gen.set_center_alignment();
}

void ScreenManager::update()
{
    switch(_current_type)
    {
    case ScreenType::Upgrade:
        _upgrade_screen.update();
        break;

    default:
        break;
    }
}
