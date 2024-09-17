#include "App.hpp"

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

App::App() {
}

void App::run() {
    Rte::ModuleManager moduleManager;
    const std::shared_ptr<Rte::Ecs> ecs = std::make_shared<Rte::Ecs>();

    // Load the graphic module
    const std::unique_ptr<Rte::IModule> graphicModule = moduleManager.loadModule("RteGraphic");
    graphicModule->init(ecs);
    graphicModule->update();
}
