#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"

#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

#include <memory>

namespace Rte::Graphic {

    class TextSystem : public System {
        public:
            TextSystem() = default;
            ~TextSystem() override = default;

            TextSystem(const TextSystem&) = delete;
            TextSystem& operator=(const TextSystem&) = delete;

            TextSystem(TextSystem&&) = default;
            TextSystem& operator=(TextSystem&&) = default;

            void init(const std::shared_ptr<Rte::Ecs>& ecs, sf::Font& font);
            void update(sf::RenderWindow& window);

        private:
            std::shared_ptr<Rte::Ecs> m_ecs;
            std::unique_ptr<sf::Text> m_text;
    };

}   // namespace Rte::Graphic
