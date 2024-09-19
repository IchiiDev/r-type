#include "RenderSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "TextureImpl.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Angle.hpp"

#include <cassert>
#include <memory>

using namespace Rte::Graphic;

void RenderSystem::init(const std::shared_ptr<Rte::Ecs>& ecs) {
    m_ecs = ecs;
}

void RenderSystem::update(sf::RenderWindow& window) {
    assert(m_ecs != nullptr && "Cannot update render system: Not initialized.");

    for (const Entity entity : m_entities) {
        const Components::Sprite& spriteComponent = m_ecs->getComponent<Components::Sprite>(entity);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);

        // Texture
        const std::shared_ptr<TextureImpl> texture = interfaceCast<TextureImpl>(spriteComponent.texture);
        sf::Sprite sprite(texture->getHandle());

        // Transform
        const sf::Angle angle = sf::degrees(transformComponent.rotation.x);
        sprite.setPosition({transformComponent.position.x, transformComponent.position.y});
        sprite.setScale({transformComponent.scale.x, transformComponent.scale.y});
        sprite.setRotation(angle);

        // Draw
        window.draw(sprite);
    }
}
