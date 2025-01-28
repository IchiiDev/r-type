#include "FlappyBird.hpp"
#include "Rte/Graphic/Components.hpp"

Rte::Entity FlappyBird::createSpriteEntity(const std::string &texturePath, Rte::Vec2<float> position, Rte::Vec2<float> scale, float rotation, int layer) {
    const Rte::Entity entity = m_ecs->createEntity();

    const uint32_t texture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(texture, texturePath.c_str()))
        throw std::runtime_error("Failed to load texture: " + std::string(texturePath));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{.position = position, .scale = scale, .rotation = rotation});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite{.textureId = texture, .layer = layer});

    return entity;
}

Rte::Entity FlappyBird::createSpriteEntityFromTexture(uint32_t texture, Rte::Vec2<float> position, Rte::Vec2<float> scale, float rotation, int layer) {
    const Rte::Entity entity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{.position = position, .scale = scale, .rotation = rotation});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite{.textureId = texture, .layer = layer});

    return entity;
}
