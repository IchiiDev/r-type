#include "ClientApp.hpp"

#include "Button.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

ClientApp::ClientApp() {
    // Init ECS
    m_ecs = std::make_shared<Rte::Ecs>();

    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type");
    m_graphicModule->setWindowSize({1280, 720});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);

    // Callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));

    // 1*1 red texture for the buttons
    const std::shared_ptr<Rte::Graphic::Texture> redTexture = m_graphicModule->createTexture();
    redTexture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});

    // Play button
    m_buttons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, redTexture, Rte::Vec2<float>(0, -40), Rte::Vec2<float>(200, 50), [&]() {
        std::cout << "Play button clicked\n";
    }));

    // Options button
    m_buttons.push_back(std::make_unique<Button>(m_ecs, m_graphicModule, redTexture, Rte::Vec2<float>(0, 40), Rte::Vec2<float>(200, 50), [&]() {
        std::cout << "Options button clicked\n";
    }));
}

void ClientApp::menuLoop() {
    while (m_running) {
        for (const std::unique_ptr<Button>& button : m_buttons)
            button->update();

        m_graphicModule->update();
    }
}

void ClientApp::run() {
    menuLoop();
}
