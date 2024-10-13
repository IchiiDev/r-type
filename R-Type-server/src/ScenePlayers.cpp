#include "Enemy.hpp"
#include "Rte/Physics/ShapeBody.hpp"
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
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numbers>

void ServerApp::updatePlayers() {
    for (auto& [playerId, player] : m_players) {
        player->update();
        for (auto& [enemyId, enemy] : m_enemies) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(player->getEntity()).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(enemy->getEntity()).shapeBody)) {
                player->takeDamage();
                destroyEnemy(enemy->getEntity());
            }
        }
    }
}
