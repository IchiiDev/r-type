#include "TextSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Angle.hpp"

#include <cassert>
#include <memory>

using namespace Rte::Graphic;

void TextSystem::init(const std::shared_ptr<Rte::Ecs>& ecs, sf::Font& font) {
    m_ecs = ecs;
    m_text = std::make_unique<sf::Text>(font, "", 30);
}

void TextSystem::update(sf::RenderWindow& window) {
    assert(m_ecs != nullptr && "Cannot update render system: Not initialized.");

    for (const Entity entity : m_entities) {
        const Components::Text& textComponent = m_ecs->getComponent<Components::Text>(entity);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);

        // Transform
        const sf::Vector2<u32> windowSize = window.getSize();
        const sf::Angle angle = sf::degrees(transformComponent.rotation);

        m_text->setPosition({
            transformComponent.position.x + static_cast<float>(textComponent.offset.x) + static_cast<float>(windowSize.x) / 2.0F,
            transformComponent.position.y + static_cast<float>(textComponent.offset.y) + static_cast<float>(windowSize.y) / 2.0F
        });
        m_text->setRotation(angle);
        m_text->setString(textComponent.text);
        m_text->setCharacterSize(textComponent.size);
        m_text->setOrigin({m_text->getLocalBounds().size.x / 2.0F, m_text->getLocalBounds().size.y / 2.0F});

        // Draw
        window.draw(*m_text);
    }
}
