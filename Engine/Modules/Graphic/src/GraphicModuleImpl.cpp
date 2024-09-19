#include "Rte/Graphic/GraphicModule.hpp"
#include "GraphicModuleImpl.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "TextureImpl.hpp"

#include <iostream>
#include <memory>

using namespace Rte::Graphic;

Rte::IModule *createModule() {
    return new GraphicModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

void GraphicModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    std::cout << "GraphicModuleImpl::init()\n";
}

void GraphicModuleImpl::update() {
    std::cout << "GraphicModuleImpl::update()\n";
}

std::unique_ptr<Texture> GraphicModuleImpl::createTexture() const { // NOLINT(readability-convert-member-functions-to-static)
    return std::make_unique<TextureImpl>();
}
