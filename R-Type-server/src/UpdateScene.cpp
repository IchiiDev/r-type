#include "ServerApp.hpp"

#include <algorithm>
#include <cstdlib>

void ServerApp::updateScene() {
    updateProjectiles();
    updateBreakables();
    updatePowerups();
    updateEnemies();
    updatePlayers();
}