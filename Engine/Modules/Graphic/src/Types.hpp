#pragma once

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Image.hpp"

namespace Rte::Graphic {

    struct TextureHandle {
        sf::Texture texture;
        sf::Image image;
    };

}   // namespace Rte::Graphic
