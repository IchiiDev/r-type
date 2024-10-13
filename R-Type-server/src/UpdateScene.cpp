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

void ServerApp::updateScene() {
    updateProjectiles();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_clock).count() > 3000) {
        createEnemy({1800, std::clamp(static_cast<float>(rand() % 1080), 100.F, 1000.F)});
    }
    updatePowerups();
    updateEnemies();
    updatePlayers();
}