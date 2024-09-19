#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <memory>

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ServerApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Graphic::GraphicModule> graphicModule = Rte::moduleCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    graphicModule->init(m_ecs);
    graphicModule->update();

    // Texture example
    const std::unique_ptr<Rte::Graphic::Texture> texture = graphicModule->createTexture();
    texture->create({100, 100});
    texture->setSmooth(true);
    texture->generateMipmap();
}
