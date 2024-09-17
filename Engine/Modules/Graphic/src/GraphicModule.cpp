#include "GraphicModule.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include <iostream>

using namespace Rte;

IModule *createModule() {
    return new GraphicModule(); // NOLINT(cppcoreguidelines-owning-memory)
}

void GraphicModule::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    std::cout << "GraphicModule::init()\n";
}

void GraphicModule::update() {
    std::cout << "GraphicModule::update()\n";
}
