#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"

#include <memory>
#include <vector>
#include <iostream>

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ServerApp::run() {
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    networkModule->init(m_ecs);
    networkModule->setUpConnection(Rte::Network::connectionType::Server, Rte::Network::connectionProtocol::TCP);
    bool result = networkModule->start_as_server("8081001");

    // Main loop
    while (true) {
        networkModule->update();
        // std::vector<Rte::Network::InputPackage> packages = networkModule->receive_as_server();
        /*
        for (const auto& package : packages) {
            std::cout << "received inputs from:" << package.player_id << std::endl;
        }
        */
    }
}
