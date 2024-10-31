#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Types.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

#include <memory>
#include <map>

namespace Rte::Graphic {

    class ButtonSystem : public System {
        public:
            ButtonSystem() = default;
            ~ButtonSystem() override = default;

            ButtonSystem(const ButtonSystem&) = delete;
            ButtonSystem& operator=(const ButtonSystem&) = delete;
            ButtonSystem(ButtonSystem&&) = default;
            ButtonSystem& operator=(ButtonSystem&&) = default;

            void init(const std::shared_ptr<Rte::Ecs>& ecs);
            void update(sf::RenderWindow& window, const std::map<uint32_t, TextureHandle> &textures);

        private:
            std::shared_ptr<Rte::Ecs> m_ecs;
    };

}   // namespace Rte::Graphic
