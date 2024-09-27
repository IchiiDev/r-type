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
    wood = 5 // Wood
};

using comportement_t = enum Comportement_e {
    // Comportement
    nothing = 0, // Air
    liquid = 1, // Liquid
    solid = 2, // Solid
    fire = 3, // Fire
    gas = 4, // Gas
    powder = 5 // Sand
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

    comportement_t comp;
};

typedef struct {
    materials_t mat;
    color_t color;
    float life_time;
} pixel_t;

const std::map<int, materials_t> matsColors = {
    {0 * 1 + 0 * 2 + 0 * 4 + 0 * 8, air},
    {53 * 1 + 35 * 2 + 0 * 4 + 255 * 8, s_wood},
    {79 * 1 + 53 * 2 + 0 * 4 + 255 * 8, d_wood},
    {222 * 1 + 158 * 2 + 65 * 4 + 255 * 8, sand},
    {0 * 1 + 200 * 2 + 255 * 4 + 120 * 8, water}
};


const std::map<materials_t, color_t> invMatsColors = {
    {air,       {0,   0,   0,   0}},
    {s_wood,    {53,  35,  0,   255}},
    {d_wood,    {79,  53,  0,   255}},
    {sand,      {222, 158, 65,   255}},
    {water,     {0,   200, 255, 120}}
};

const std::map<materials_t, material_def> mats = {
    {air, {0.F, 0.F, true, 0.F, 0.F, 0.F, false, nothing}},
    {s_wood, {1.F, .3F, false, .10F, .10F, .0F, false, solid}},
    {d_wood, {1.F, .3F, true, .10F, .10F, .0F, false, solid}},
    {sand, {1.F, .3F, true, .0F, .0F, .0F, false, powder}},
    {water, {1.F, .3F, true, .0F, .0F, .0F, false, liquid}},
};
