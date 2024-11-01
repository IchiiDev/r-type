#pragma once

#include "Rte/Common.hpp"
#include "Rte/Graphic/GraphicModule.hpp"

#include <cstdint>

namespace Rte::Graphic::Components {

    struct Sprite {
        uint32_t textureId{};
        Rte::Vec2<int> offset{};
        int layer = 0;  // Must be < 7
    };

    struct Button {
        ButtonState state = ButtonState::NONE;
    };

    struct Text {
        Rte::Vec2<uint32_t> offset{};
        std::string text;
        int size{};
    };

}   // namespace Rte::Graphic::Components
