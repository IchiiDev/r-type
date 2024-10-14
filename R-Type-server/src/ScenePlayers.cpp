#include "Enemy.hpp"
#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>
#include "Rte/Network/NetworkModuleTypes.hpp"

void ServerApp::updatePlayers() {
    for (auto& [playerId, player] : m_players) {
        if (player->getHealth() <= 0) {
            m_networkModuleServer->deletePlayer(m_ecs->getComponent<Rte::BasicComponents::UidComponents>(player->getEntity()), playerId);
            return;
        }
        player->update();
        for (auto& [enemyId, enemy] : m_enemies) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(player->getEntity()).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(enemy->getEntity()).shapeBody)) {
                player->takeDamage(20);
                destroyEnemy(enemy->getEntity());
                return;
            }
        }
    }
}
