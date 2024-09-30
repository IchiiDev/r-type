#pragma once

#include "Rte/Common.hpp"

#include <map>

namespace Rte::Physics {

    enum class MaterialType  {
        AIR,

        // Rigidbody materials
        STATIC_WOOD,
        DYNAMIC_WOOD,

        // SandBox materials
        SAND,
        WATER,
        ACID
    };

    enum class Comportement {
        NOTHING = 0,
        LIQUID = 1,
        SOLID = 2,
        FIRE = 3,
        GAS = 4,
        POWDER = 5,
    };

    struct Color {
        int r;
        int g;
        int b;
        int a;
    };

    struct MaterialDef {
        // Box2D variables
        float density;
        float friction;
        bool isDynamic;

        // Material properties
        float igniteOdds;
        float burn_odds;
        float glow;
        bool isAcidResistant;

        Comportement comp;
    };

    struct Pixel {
        MaterialType mat;
        Color color;
        float life_time;
    };

    struct Particle {
        Pixel pixel;
        Rte::Vec2<int> pos;
        Rte::Vec2<float> vel;
    };

    const std::map<int, MaterialType> matColors = {
        {0   * 1 + 0   * 2 + 0   * 4 + 0   * 8, MaterialType::AIR},
        {53  * 1 + 35  * 2 + 0   * 4 + 255 * 8, MaterialType::STATIC_WOOD},
        {79  * 1 + 53  * 2 + 0   * 4 + 255 * 8, MaterialType::DYNAMIC_WOOD},
        {222 * 1 + 158 * 2 + 65  * 4 + 255 * 8, MaterialType::SAND},
        {100 * 1 + 161 * 2 + 194 * 4 + 120 * 8, MaterialType::WATER},
        {116 * 1 + 194 * 2 + 100 * 4 + 120 * 8, MaterialType::ACID}
    };


    const std::map<MaterialType, Color> invMatColors = {
        {MaterialType::AIR,          {0,   0,   0,   0}},
        {MaterialType::STATIC_WOOD,  {53,  35,  0,   255}},
        {MaterialType::DYNAMIC_WOOD, {79,  53,  0,   255}},
        {MaterialType::SAND,         {222, 158, 65,  255}},
        {MaterialType::WATER,        {100, 161, 194, 120}},
        {MaterialType::ACID,         {116, 194, 100, 120}}
    };

    const std::map<MaterialType, MaterialDef> mats = {
        {MaterialType::AIR,          {0.F, 0.F, true,  0.F, 0.F, 0.F,  false, Comportement::NOTHING}},
        {MaterialType::STATIC_WOOD,  {1.F, .3F, false, .10F, .3F, .0F, false, Comportement::SOLID}},
        {MaterialType::DYNAMIC_WOOD, {1.F, .3F, true,  .10F, .3F, .0F, false, Comportement::SOLID}},
        {MaterialType::SAND,         {1.F, .3F, true,  .0F, .0F, .0F,  false, Comportement::POWDER}},
        {MaterialType::WATER,        {1.F, .3F, true,  .0F, .0F, .0F,  false, Comportement::LIQUID}},
        {MaterialType::ACID,         {1.F, .3F, true,  .0F, .0F, .0F,  true,  Comportement::LIQUID}}
    };

}   // namespace Rte::Physics
