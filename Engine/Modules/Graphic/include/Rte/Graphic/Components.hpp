#pragma once

#include "Texture.hpp"

#include <memory>

namespace Rte::Graphic::Components {

    class Sprite {
        std::shared_ptr<Texture> texture;
    };

}   // namespace Rte::Graphic::Components
