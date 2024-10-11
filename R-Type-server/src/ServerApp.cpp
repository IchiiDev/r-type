#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <iostream>

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ServerApp::run() {
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    const std::shared_ptr<Rte::Network::NetworkModuleServer> networkModuleServer = networkModule->getServer();

    networkModuleServer->init(m_ecs);
    networkModuleServer->start(123456);

    while (true) {
        networkModuleServer->update();
    }
}
