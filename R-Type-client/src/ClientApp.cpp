#include "ClientApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"


#include <memory>
#include <vector>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ClientApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    networkModule->init(m_ecs);
    networkModule->setUpConnection(Rte::Network::connectionType::Client, Rte::Network::connectionProtocol::TCP);
    networkModule->connect_as_client("51.38.81.49", 80);


    // Main loop
    while (true) {
        networkModule->update();
        // networkModule->send_data(Rte::Network::InputPackage{.player_id = 69, .jump = true, .right = false, .left = false, .shot = true, .shoot_angle = 90});
    }
}
