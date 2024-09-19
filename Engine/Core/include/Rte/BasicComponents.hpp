#pragma once

#include "Rte/Common.hpp"

namespace Rte::BasicComponents {

    struct Transform {
        Vec3<float> position{0, 0, 0};
        Vec3<float> rotation{0, 0, 0};
        Vec3<float> scale{1, 1, 1};
    };

}   // namespace Rte::BasicComponents
