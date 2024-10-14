#pragma once

#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <memory>

namespace Rte::Graphic::Components {

    struct Sprite {
        std::shared_ptr<Texture> texture;
        int layer = 0;  // Must be < 7
    };

    struct Button {
        ButtonState state = ButtonState::NONE;
    };

    struct Text {
        std::string text;
        int size;
    };

}   // namespace Rte::Graphic::Components
