#include "ButtonSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Mouse.hpp"

#include <cassert>
#include <memory>

using namespace Rte::Graphic;

void ButtonSystem::init(const std::shared_ptr<Rte::Ecs>& ecs) {
    m_ecs = ecs;
}

void ButtonSystem::update(sf::RenderWindow& window) {
    assert(m_ecs != nullptr && "Cannot update render system: Not initialized.");

    for (const Entity entity : m_entities) {
        // Get components
        Components::Button& buttonComponent = m_ecs->getComponent<Components::Button>(entity);
        const Components::Sprite& spriteComponent = m_ecs->getComponent<Components::Sprite>(entity);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);


        // Check if the button is hovered or clicked
        const Vec2<u16> textureSize = spriteComponent.texture->getSize();
        const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        const sf::Vector2<u32> windowSize = window.getSize();
        const Vec2<u16> normalizedTransformPosition = {
            static_cast<u16>(transformComponent.position.x + (windowSize.x / 2)),
            static_cast<u16>(transformComponent.position.y + (windowSize.y / 2))
        };
        const Vec2<u16> normalizedScale = {
            static_cast<u16>(transformComponent.scale.x * textureSize.x),
            static_cast<u16>(transformComponent.scale.y * textureSize.y)
        };

        const Vec2<u16> topLeft = {
            static_cast<u16>(normalizedTransformPosition.x - (normalizedScale.x * textureSize.x / 2)),
            static_cast<u16>(normalizedTransformPosition.y - (normalizedScale.y * textureSize.y / 2))
        };
        const Vec2<u16> bottomRight = {
            static_cast<u16>(normalizedTransformPosition.x + (normalizedScale.x / 2)),
            static_cast<u16>(normalizedTransformPosition.y + (normalizedScale.y / 2))
        };

        const bool isHovered = mousePosition.x >= topLeft.x && mousePosition.x <= bottomRight.x &&
                               mousePosition.y >= topLeft.y && mousePosition.y <= bottomRight.y;


        if (isHovered) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                buttonComponent.state = ButtonState::PRESSED;
            else
                buttonComponent.state = ButtonState::HOVERED;
        } else
            buttonComponent.state = ButtonState::NONE;
    }
}