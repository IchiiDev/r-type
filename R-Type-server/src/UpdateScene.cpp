#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include <iostream>

void ServerApp::destroyProjectile(const Rte::Entity& projectile) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(projectile);
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(m_entities->begin() + j);
            break;
        }
    }
    m_ecs->destroyEntity(projectile);
    for (size_t i = 0; i < m_projectiles.size(); i++)
        if (*m_projectiles[i] == projectile)
            m_projectiles.erase(m_projectiles.begin() + i);
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}

void ServerApp::updateScene() {
    for (size_t i = 0; i < m_projectiles.size(); i++) {
        if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(m_rightWall).shapeBody)) {
            destroyProjectile(*m_projectiles[i]);
        }
    }
}