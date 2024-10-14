#include "ServerApp.hpp"

#include <algorithm>
#include <cstdlib>

void ServerApp::updateScene() {
    updateProjectiles();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_EnemyClock).count() > 3000) {
        createEnemy({1800, std::clamp(static_cast<float>(rand() % 1080), 100.F, 1000.F)});
        m_EnemyClock = std::chrono::high_resolution_clock::now();
    }
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_ObstacleClock).count() > 4000) {
        createObstacle({1800, std::clamp(static_cast<float>(rand() % 1080), 100.F, 1000.F)});
        m_ObstacleClock = std::chrono::high_resolution_clock::now();
    }
    updatePowerups();
    updateEnemies();
    updateObstacles();
    updatePlayers();
}