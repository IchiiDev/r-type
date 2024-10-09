#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"

#include <memory>
#include <vector>
#include <iostream>

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ServerApp::run() {
}
