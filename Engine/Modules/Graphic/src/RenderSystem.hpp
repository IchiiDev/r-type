#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Shader.hpp"

#include <memory>

namespace Rte::Graphic {

    class RenderSystem : public System {
        public:
            void init(const std::shared_ptr<Rte::Ecs>& ecs);
            void update(sf::RenderWindow& window, sf::Shader& shader);

        private:
            std::shared_ptr<Rte::Ecs> m_ecs;
    };

}   // namespace Rte::Graphic
