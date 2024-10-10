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
#include "Rte/Network/NetworkModuleClient.hpp"


#include <memory>
#include <vector>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ClientApp::run() {
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    const std::shared_ptr<Rte::Network::NetworkModuleClient> networkModuleClient = networkModule->getClient();

    while(true) {
        // networkModuleClient->updateInputs(PackedInput input)
        networkModuleClient->update();
    }
}
