#include "App.hpp"

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/IModule.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

Rte::Ecs ecs;   // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

App::App() {
}

void App::run() {
    ecs.createEntity();

    ModuleManager moduleManager;
    std::unique_ptr<IModule> graphicModule = moduleManager.loadModule("RteGraphic");
    graphicModule->update();
}
