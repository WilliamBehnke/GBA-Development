#include "customization_screen.h"
#include "character_colors.h"
#include "character_assets.h"

#include "bn_keypad.h"

CustomizationScreen::CustomizationScreen() :
    _preview(bn::fixed_point(-80, 0), _model.appearance())
{
    _model.mark_style_dirty();
    _model.mark_colors_dirty();

    _apply_to_preview();
    _refresh_ui();
}

void CustomizationScreen::update()
{
    _handle_input();
    _apply_to_preview();
    _preview.update();
}

void CustomizationScreen::_handle_input()
{
    bool ui_needs_refresh = false;

    // Bumpers switch tabs
    if(bn::keypad::l_pressed())
    {
        _menu.move_tab(-1);
        ui_needs_refresh = true;
    }
    else if(bn::keypad::r_pressed())
    {
        _menu.move_tab(1);
        ui_needs_refresh = true;
    }

    auto handle_move = [&](int drow, int dcol)
    {
        bool changed = _menu.move_selection(_model.appearance(), drow, dcol);

        if(!changed)
        {
            return;
        }

        ui_needs_refresh = true;

        if(_menu.is_style_tab())
        {
            _model.mark_style_dirty();
        }
        else if(_menu.is_color_tab())
        {
            _model.mark_colors_dirty();
        }
    };

    // D-pad moves selection in the current tab grid
    if(bn::keypad::left_pressed())
    {
        handle_move(0, -1);
    }
    else if(bn::keypad::right_pressed())
    {
        handle_move(0, 1);
    }
    else if(bn::keypad::up_pressed())
    {
        handle_move(-1, 0);
    }
    else if(bn::keypad::down_pressed())
    {
        handle_move(1, 0);
    }

    // Rotate character with A
    if(bn::keypad::a_pressed())
    {
        _rotate_character(1);
    }

    // Toggle animation with B
    if(bn::keypad::b_pressed())
    {
        _preview.toggle_animation();
    }

    if(ui_needs_refresh)
    {
        _refresh_ui();
    }

    if(bn::keypad::start_pressed())
    {
        _done = true;
    }
}

void CustomizationScreen::_rotate_character(int delta_steps)
{
    CharacterAppearance& a = _model.appearance();

    int d = static_cast<int>(a.direction);
    d = (d + delta_steps + 4) % 4;
    a.direction = static_cast<FacingDirection>(d);

    _preview.set_direction(a.direction);
    _preview.refresh();
    _preview.set_scale(2);
}

void CustomizationScreen::_apply_to_preview()
{
    if(!_model.style_dirty() && !_model.colors_dirty())
    {
        return;
    }

    _preview.refresh();
    _preview.set_scale(2);

    _model.clear_dirty();
}

void CustomizationScreen::_refresh_ui()
{
    _menu.draw(appearance());
}
