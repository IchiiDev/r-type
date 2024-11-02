#pragma once

#include "Rte/Common.hpp"

#include <cstdint>

namespace Rte::BasicComponents {

    struct Transform {
        Vec2<float> position{0, 0};
        Vec2<float> scale{1, 1};
        float rotation{0};
    };

    struct UidComponents {
        uint32_t uid;
    };

    struct Tag {
        std::string tag;
    };

}   // namespace Rte::BasicComponents
