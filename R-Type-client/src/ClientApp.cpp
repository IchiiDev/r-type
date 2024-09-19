#include "ClientApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ClientApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Graphic::GraphicModule> graphicModule = Rte::moduleCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    graphicModule->init(m_ecs);
    graphicModule->setWindowTitle("R-Type");
    graphicModule->setWindowSize({1280, 720});


    // Callback to close the window
    bool running = true;
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::Window::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            running = false;
        }
    ));


    // Main loop
    while (running) {
        graphicModule->update();
    }
}
