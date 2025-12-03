#ifndef CUSTOMIZATION_SCREEN_H
#define CUSTOMIZATION_SCREEN_H

// ---------------------------------------------------------------------------
// customization_screen.h
// Screen controller for character customization (MVC controller + view glue).
// ---------------------------------------------------------------------------

#include "character_model.h"
#include "character_preview.h"
#include "customization_menu.h"

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_optional.h"

class CustomizationScreen
{
public:
    CustomizationScreen();

    void update();

    bool done() const
    {
        return _done;
    }

    const CharacterAppearance& appearance() const
    {
        return _model.appearance();
    }

private:
    // Input + state (controller)
    void _handle_input();
    void _rotate_character(int delta_steps);

    // Rendering / preview (sync model -> view)
    void _apply_to_preview();
    void _refresh_ui();

    // MVC model
    CharacterModel _model;

    // Views
    CharacterPreview _preview;
    CustomizationMenu _menu;

    bool _done = false;
};

#endif // CUSTOMIZATION_SCREEN_H
