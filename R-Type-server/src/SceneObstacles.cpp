#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>

void ServerApp::createObstacle(Rte::Vec2<float> pos) {
    // Add obstacle to the entities list
    Rte::Entity obstacle = m_ecs->createEntity();
    // Load texture
    uint32_t obstacleTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(obstacleTexture, "../assets/asteroid.png"))
        throw std::runtime_error("Failed to load texture: \"../assets/asteroid.png\"");

    // Add obstacle components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(obstacle, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(obstacle, Rte::Graphic::Components::Sprite{obstacleTexture});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(obstacle, Rte::BasicComponents::Transform{
        .position = {pos.x, pos.y},
        .scale = {2, 2},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(obstacle, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {120 * 2, 0}, 10, 0,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(obstacle).position, 0, false, false,
        Rte::Physics::ShapeType::CIRCLE
    )});

    // Add obstacle to the obstacles list
    m_obstacles.push_back(std::make_unique<Rte::Entity>(obstacle));

    // Add obstacle to the entities list
    m_entities->emplace_back(obstacle);

    // Load texture and add to new entities textures
    uint32_t texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(obstacle).textureId;
    const Rte::u8* texturePixels = m_graphicModule->getTexturePixels(texture);
    const Rte::Vec2<Rte::u16> textureSize = m_graphicModule->getTextureSize(texture);
    std::vector<Rte::u8> pixelsVector(texturePixels, texturePixels + static_cast<ptrdiff_t>(textureSize.x * textureSize.y) * 4);

    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = textureSize;
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[obstacle] = packedTexture;
}

void ServerApp::updateObstacles() {
    for (size_t i = 0; i < m_obstacles.size(); i++) {
        for (auto& projectile : m_projectiles) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_obstacles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(*projectile).shapeBody)) {
                destroyObstacle(*m_obstacles[i]);
                destroyProjectile(*projectile);
                return;
            }
        }
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_obstacles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_leftWall).shapeBody)) {
            destroyObstacle(*m_obstacles[i]);
            return;
        }
        m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_obstacles[i]).shapeBody, {-5, 0});
    }
}

void ServerApp::destroyObstacle(const Rte::Entity& obstacle) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(obstacle);
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(std::next(m_entities->begin(), static_cast<std::ptrdiff_t>(j)));
            break;
        }
    }
    m_ecs->destroyEntity(obstacle);
    for (size_t i = 0; i < m_obstacles.size(); i++)
        if (*m_obstacles[i] == obstacle)
            m_obstacles.erase(std::next(m_obstacles.begin(), static_cast<std::ptrdiff_t>(i)));
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}
