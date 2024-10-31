#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"

#include <cmath>
#include <memory>
#include <vector>
#include <numbers>

void ServerApp::createEnemy(Rte::Vec2<float> pos) {
    // New enemy creation
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>(m_ecs, m_graphicModule, m_physicsModule, m_currentUid++, pos);

    // Add enemy to the entities list
    m_entities->emplace_back(enemy->getEntity());

    // Load texture and add to new entities textures
    uint32_t texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(enemy->getEntity()).textureId;
    std::vector<Rte::u8> pixelsVector(m_graphicModule->getTexturePixels(texture), m_graphicModule->getTexturePixels(texture) + static_cast<ptrdiff_t>(m_graphicModule->getTextureSize(texture).x * m_graphicModule->getTextureSize(texture).y) * 4);

    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = m_graphicModule->getTextureSize(texture);
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[enemy->getEntity()] = packedTexture;

    // Add enemy to the enemies list
    m_enemies.insert({enemy->getEntity(), std::move(enemy)});
}

void ServerApp::updateEnemies() {
    for (auto& [enemyId, enemy] : m_enemies) {
        if (enemy->getHealth() <= 0) {
            destroyEnemy(enemy->getEntity());
            return;
        }
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(enemy->getEntity()).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_leftWall).shapeBody)) {
            destroyEnemy(enemy->getEntity());
            return;
        }
        enemy->update();
        enemy->move({-5, 0});
        Rte::Entity projectile = enemy->shoot(std::numbers::pi_v<float>);
        if (projectile != 0)
            createProjectile(projectile);
    }
}

void ServerApp::destroyEnemy(const Rte::Entity& enemy) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(enemy);
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(std::next(m_entities->begin(), static_cast<std::vector<Rte::Entity>::difference_type>(j)));
            break;
        }
    }
    m_ecs->destroyEntity(enemy);
    m_enemies.erase(enemy);
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}
