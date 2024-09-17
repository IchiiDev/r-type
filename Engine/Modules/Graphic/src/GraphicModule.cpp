#include "GraphicModule.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/IModule.hpp"

#include <iostream>

extern Rte::Ecs ecs;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

IModule *createModule() {
    return new GraphicModule(); // NOLINT(cppcoreguidelines-owning-memory)
}

void GraphicModule::update() {
    ecs.createEntity();
    std::cout << "GraphicModule::update()\n";
}
