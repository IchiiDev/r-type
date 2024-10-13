#include "Enemy.hpp"
#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>

void ServerApp::updatePlayers() {
    for (auto& [playerId, player] : m_players) {
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
