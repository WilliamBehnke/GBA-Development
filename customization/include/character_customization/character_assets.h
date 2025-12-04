#ifndef CHARACTER_ASSETS_H
#define CHARACTER_ASSETS_H

// ---------------------------------------------------------------------------
// character_assets.h
// Sprite assets for all character customization parts.
// ---------------------------------------------------------------------------

#include "bn_sprite_item.h"

// Character component sprite sheets
#include "bn_sprite_items_skin_0.h"
#include "bn_sprite_items_hair_0.h"
#include "bn_sprite_items_hair_1.h"
#include "bn_sprite_items_eyes_0.h"
#include "bn_sprite_items_top_0.h"
#include "bn_sprite_items_bottom_0.h"

// ---------------------------------------------------------
// Enums for component choices
// ---------------------------------------------------------

enum class BodyType : int
{
    Skin0 = 0,
    Count
};

enum class HairType : int
{
    Hair0 = 0,
    Hair1 = 1,
    Count
};

enum class EyesType : int
{
    Eyes0 = 0,
    Count
};

enum class TopType : int
{
    Top0 = 0,
    Count
};

enum class BottomType : int
{
    Bottom0 = 0,
    Count
};

// ---------------------------------------------------------
// Sprite option arrays indexed by enums
// ---------------------------------------------------------

constexpr int k_body_count   = static_cast<int>(BodyType::Count);
constexpr int k_hair_count   = static_cast<int>(HairType::Count);
constexpr int k_eyes_count   = static_cast<int>(EyesType::Count);
constexpr int k_top_count    = static_cast<int>(TopType::Count);
constexpr int k_bottom_count = static_cast<int>(BottomType::Count);

inline constexpr const bn::sprite_item* k_body_type_options[] =
{
    &bn::sprite_items::skin_0,
};
static_assert(sizeof(k_body_type_options)/sizeof(void*) == k_body_count);

inline constexpr const bn::sprite_item* k_hair_options[] =
{
    &bn::sprite_items::hair_0,
    &bn::sprite_items::hair_1,
};
static_assert(sizeof(k_hair_options)/sizeof(void*) == k_hair_count);

inline constexpr const bn::sprite_item* k_eyes_options[] =
{
    &bn::sprite_items::eyes_0,
};
static_assert(sizeof(k_eyes_options)/sizeof(void*) == k_eyes_count);

inline constexpr const bn::sprite_item* k_top_options[] =
{
    &bn::sprite_items::top_0,
};
static_assert(sizeof(k_top_options)/sizeof(void*) == k_top_count);

inline constexpr const bn::sprite_item* k_bottom_options[] =
{
    &bn::sprite_items::bottom_0,
};
static_assert(sizeof(k_bottom_options)/sizeof(void*) == k_bottom_count);

#endif // CHARACTER_ASSETS_H
