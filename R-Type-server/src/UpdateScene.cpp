#include "Rte/BasicComponents.hpp"
#include "ServerApp.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

void ServerApp::updateScene() {
    static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    const std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    

    updateTerrain();
    updateProjectiles();
    updateBreakables();
    updatePowerups();
    updateEnemies();
    updatePlayers();
}