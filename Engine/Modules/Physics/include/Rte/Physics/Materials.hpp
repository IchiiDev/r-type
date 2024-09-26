#pragma once

#include <map>
#include <string>
#include <vector>

using materials_t = enum Materials_e {
    air = 0, // Air
    s_wood = 1, // Static wood
    d_wood = 2, // Dynamic wood
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
};

const std::map<int, materials_t> matColors = {
    {0 * 1 + 0 * 2 + 0 * 4 + 0 * 8, air},
    {53 * 1 + 35 * 2 + 0 * 4 + 255 * 8, s_wood},
    {79 * 1 + 53 * 2 + 0 * 4 + 255 * 8, d_wood}
};

const std::map<materials_t, material_def> materials = {
    {air, {0.F, 0.F, true, 0.F, 0.F, 0.F, false}},
    {s_wood, {1.F, .3F, false, .10F, .10F, .0F, false}},
    {d_wood, {1.F, .3F, true, .10F, .10F, .0F, false}}
};
