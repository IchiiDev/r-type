#pragma once

#include "Rte/Common.hpp"

#define PPM 10.0F

namespace Rte::Physics {

    struct PixelCringe {
        Rte::u8 r;
        Rte::u8 g;
        Rte::u8 b;
        Rte::u8 a;
        Rte::Vec2<float> pos;
    };

}   // namespace Rte::Physics
