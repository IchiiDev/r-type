#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>

void ServerApp::createProjectile(Rte::Entity projectile) {
    // Add projectile to the entities list
    m_ecs->addComponent(projectile, Rte::BasicComponents::UidComponents{m_currentUid++});
    
    // Add projectile to the projectiles list
    m_projectiles.push_back(std::make_unique<Rte::Entity>(projectile));

    // Add projectile to the entities list
    m_entities->emplace_back(projectile);

    // Load texture and add to new entities textures
    auto texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(projectile).texture;
    std::vector<Rte::u8> pixelsVector(texture->getPixels(), texture->getPixels() + static_cast<ptrdiff_t>(texture->getSize().x * texture->getSize().y) * 4);
    
    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = texture->getSize();
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[projectile] = packedTexture;
}

void ServerApp::updateProjectiles() {
    for (size_t i = 0; i < m_projectiles.size(); i++) {
        // Destroy projectile if it collides with any of the walls
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_leftWall).shapeBody)) {
            destroyProjectile(*m_projectiles[i]);
            continue;
        }
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_rightWall).shapeBody)) {
            destroyProjectile(*m_projectiles[i]);
            continue;
        }
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_topWall).shapeBody)) {
            destroyProjectile(*m_projectiles[i]);
            continue;
        }
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_bottomWall).shapeBody)) {
            destroyProjectile(*m_projectiles[i]);
            continue;
        }
        for (auto& [enemyId, enemy] : m_enemies) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(enemy->getEntity()).shapeBody)) {
                enemy->takeDamage();
                if (enemy->getHealth() <= 0 && rand() % 5 == 1) {
                    Rte::Vec2<float> pos = enemy->getPos();
                    destroyEnemy(enemy->getEntity());
                    createPowerup(pos);
                }
                destroyProjectile(*m_projectiles[i]);
                return;
            }
        }
        for (auto& [playerId, player] : m_players) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(player->getEntity()).shapeBody)) {
                player->takeDamage(20);
                destroyProjectile(*m_projectiles[i]);
                return;
            }
        }
    }
}

void ServerApp::destroyProjectile(const Rte::Entity& projectile) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(projectile);
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(m_entities->begin() + static_cast<std::vector<Rte::Entity>::difference_type>(j));
            break;
        }
    }
    m_ecs->destroyEntity(projectile);
    for (size_t i = 0; i < m_projectiles.size(); i++)
        if (*m_projectiles[i] == projectile)
            m_projectiles.erase(m_projectiles.begin() + static_cast<std::vector<std::unique_ptr<Rte::Entity>>::difference_type>(i));
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}
