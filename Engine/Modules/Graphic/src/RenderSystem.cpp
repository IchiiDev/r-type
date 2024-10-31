#include "RenderSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/System/Vector2.hpp"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Angle.hpp"
#include "Types.hpp"

#include <cassert>
#include <memory>

using namespace Rte::Graphic;

void RenderSystem::init(const std::shared_ptr<Rte::Ecs>& ecs) {
    m_ecs = ecs;
}

void RenderSystem::update(sf::RenderWindow& window, sf::Shader& shader, int layerCount, const std::map<uint32_t, TextureHandle> &textures) {
    assert(m_ecs != nullptr && "Cannot update render system: Not initialized.");

    for (int currentLayer = layerCount; currentLayer >= 0; currentLayer--) {
        for (const Entity entity : m_entities) {
            // Draw only entities of the current layer
            const Components::Sprite& spriteComponent = m_ecs->getComponent<Components::Sprite>(entity);
            if (spriteComponent.layer != currentLayer)
                continue;

            // Get transform component
            const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);

            // Texture
            const TextureHandle& textureHandle = textures.at(spriteComponent.textureId);
            sf::Sprite sprite(textureHandle.texture);

            // Set sprite properties
            const sf::Vector2<u32> windowSize = window.getSize();
            const sf::Angle angle = sf::degrees(transformComponent.rotation);

            sprite.setPosition({
                transformComponent.position.x + static_cast<float>(spriteComponent.offset.x) + static_cast<float>(windowSize.x) / 2.0F,
                transformComponent.position.y + static_cast<float>(spriteComponent.offset.y) + static_cast<float>(windowSize.y) / 2.0F
            });
            sprite.setOrigin({static_cast<float>(textureHandle.texture.getSize().x) / 2.F, static_cast<float>(textureHandle.texture.getSize().y) / 2.F});
            sprite.setScale({transformComponent.scale.x, transformComponent.scale.y});
            sprite.setRotation(angle);

            // Draw
            window.draw(sprite, &shader);
        }
    }
}
