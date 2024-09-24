#include "ServerApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <vector>
#include <iostream>

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ServerApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Graphic::GraphicModule> graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    graphicModule->init(m_ecs);
    graphicModule->setWindowTitle("R-Type");
    graphicModule->setWindowSize({1280, 720});
    graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);

    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    networkModule->init(m_ecs);
    networkModule->setUpConnection(Rte::Network::connectionType::Server, Rte::Network::connectionProtocol::TCP);
    bool result = networkModule->start_as_server("8081");

    // Creation of a 1*1 red texture
    const std::shared_ptr<Rte::Graphic::Texture> texture = graphicModule->createTexture();
    texture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});


    // Creation of a drawable entity
    constexpr Rte::Vec2<float> entityScale = {100, 100};
    const Rte::Vec2<Rte::u16> windowSize = graphicModule->getWindowSize();
    const Rte::Entity entity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite(texture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{
        .position = {
            (static_cast<float>(windowSize.x) / 2) - (entityScale.x / 2),
            (static_cast<float>(windowSize.y) / 2) - (entityScale.y / 2)
        },
        .scale = entityScale,
        .rotation = 0
    });


    // Callback to close the window
    bool running = true;
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::Window::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            running = false;
        }
    ));


    // Callback to move the sprite and make it at the center of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::Window::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const float width = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::WIDTH);
            const float height = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::HEIGHT);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
            transform.position.x = (width / 2) - (entityScale.x / 2);
            transform.position.y = (height / 2) - (entityScale.y / 2);
        }
    ));


    // Main loop
    while (running) {
        graphicModule->update();
        networkModule->update();
        networkModule->receive_as_server();
    }
}
