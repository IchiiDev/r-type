#include "ServerApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <iostream>
#include <memory>
#include <vector>

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
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            running = false;
        }
    ));


    // Callback to print the key pressed
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::KEY_PRESSED, [&](Rte::Event& event) {
        const Rte::Graphic::Key key = event.getParameter<Rte::Graphic::Key>(Rte::Graphic::Events::Params::KEY_PRESSED);
        std::cout << "Key pressed: " << static_cast<int>(key) << "\n";
    }));

    // Callback to print mouse button pressed
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::MOUSE_BUTTON_PRESSED, [&](Rte::Event& event) {
        const Rte::Graphic::MouseButton button = event.getParameter<Rte::Graphic::MouseButton>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED);
        const Rte::Vec2<Rte::u16> position = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED_POSITION);
        std::cout << "Mouse button pressed: " << static_cast<int>(button) << " at position (" << position.x << ", " << position.y << ")\n";
    }));


    // Callback to move the sprite and make it at the center of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const Rte::Vec2<Rte::u16> newSize = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::NEW_WINDOW_SIZE);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
            transform.position.x = (static_cast<float>(newSize.x) / 2) - (entityScale.x / 2);
            transform.position.y = (static_cast<float>(newSize.y) / 2) - (entityScale.y / 2);
        }
    ));


    // Main loop
    while (running) {
        graphicModule->update();
    }
}
