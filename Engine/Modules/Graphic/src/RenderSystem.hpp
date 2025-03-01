#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Types.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Shader.hpp"

#include <memory>
#include <cstdint>
#include <map>

namespace Rte::Graphic {

    class RenderSystem : public System {
        public:
            RenderSystem() = default;
            ~RenderSystem() override = default;

            RenderSystem(const RenderSystem&) = delete;
            RenderSystem& operator=(const RenderSystem&) = delete;

            RenderSystem(RenderSystem&&) = default;
            RenderSystem& operator=(RenderSystem&&) = default;

            void init(const std::shared_ptr<Rte::Ecs>& ecs);
            void update(sf::RenderWindow& window, sf::Shader& shader, int layerCount, const std::map<uint32_t, TextureHandle> &textures);

        private:
            std::shared_ptr<Rte::Ecs> m_ecs = nullptr;
    };

}   // namespace Rte::Graphic
