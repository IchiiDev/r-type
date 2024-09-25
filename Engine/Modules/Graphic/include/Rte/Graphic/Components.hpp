#pragma once

#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <memory>

namespace Rte::Graphic::Components {

    struct Sprite {
        std::shared_ptr<Texture> texture;
    };

    struct Button {
        ButtonState state = ButtonState::NONE;
    };

}   // namespace Rte::Graphic::Components
