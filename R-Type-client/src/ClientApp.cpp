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

#include <memory>
#include <vector>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ClientApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Graphic::GraphicModule> graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    graphicModule->init(m_ecs);
    graphicModule->setWindowTitle("R-Type");
    graphicModule->setWindowSize({1280, 720});


    // Creation of a 1*1 red texture
    const std::shared_ptr<Rte::Graphic::Texture> texture = graphicModule->createTexture();
    texture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});


    // Creation of a drawable entity
    const Rte::Entity entity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite(texture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{
        .position = {500, 500, 500},
        .rotation = {0, 0, 0},
        .scale = {100, 100, 100}
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
            const Rte::u16 width = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::WIDTH);
            const Rte::u16 height = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::HEIGHT);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
            transform.position.x = (width / 2.0) - 50;
            transform.position.y = (height / 2.0) - 50;
        }
    ));


    // Main loop
    while (running) {
        graphicModule->update();
    }
}
