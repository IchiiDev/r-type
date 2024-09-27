#pragma once

#include <map>

using materials_t = enum Materials_e {
    air = 0, // Air
    
    // Rigidbody materials
    s_wood = 1, // Static wood
    d_wood = 2, // Dynamic wood

    // SandBox materials
    sand = 3, // Sand
    water = 4, // Water
};

using color_t = struct {
    int r;
    int g;
    int b;
    int a;
};

using material_def = struct {
    // Box2D variables
    float density;
    float friction;
    bool is_dynamic;
    
    // Material properties
    float ignite_odds;
    float burn_odds;
    float glow;
    bool acid_resistant;
    bool is_liquid;
    bool is_sandlike;
    bool is_solidlike;
};

using pixel_t = struct {
    materials_t material;
    color_t color;
};

const std::map<int, materials_t> matColors = {
    {0 * 1 + 0 * 2 + 0 * 4 + 0 * 8, air},
    {53 * 1 + 35 * 2 + 0 * 4 + 255 * 8, s_wood},
    {79 * 1 + 53 * 2 + 0 * 4 + 255 * 8, d_wood},
    {244 * 1 + 255 * 2 + 0 * 4 + 255 * 8, sand},
    {0 * 1 + 200 * 2 + 255 * 4 + 255 * 8, water}
};


const std::map<materials_t, color_t> invMatColors = {
    {air,       {0,   0,   0,   0}},
    {s_wood,    {53,  35,  0,   255}},
    {d_wood,    {79,  53,  0,   255}},
    {sand,      {244, 255, 0,   255}},
    {water,     {0,   200, 255, 255}}
};

const std::map<materials_t, material_def> materials = {
    {air, {0.F, 0.F, true, 0.F, 0.F, 0.F, false, false, false, false}},
    {s_wood, {1.F, .3F, false, .10F, .10F, .0F, false, false, false, true}},
    {d_wood, {1.F, .3F, true, .10F, .10F, .0F, false, false, false, true}},
    {sand, {1.F, .3F, true, .0F, .0F, .0F, false, false, true, false}},
    {water, {1.F, .3F, true, .0F, .0F, .0F, false, true, false, false}}
};
