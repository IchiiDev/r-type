#pragma once

#include "Rte/Common.hpp"

namespace Rte::BasicComponents {

    struct Transform {
        Vec2<float> position{0, 0};
        Vec2<float> scale{1, 1};
        float rotation{0};
    };

    struct UidComponents {
        uid_t uid;
    };

}   // namespace Rte::BasicComponents
