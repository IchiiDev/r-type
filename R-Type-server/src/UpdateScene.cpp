#include "ServerApp.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

void ServerApp::updateScene() {
    updateProjectiles();
    updateBreakables();
    updatePowerups();
    updateEnemies();
    updatePlayers();
}