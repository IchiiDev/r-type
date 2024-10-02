#include "ClientApp.hpp"

#include "Button.hpp"
#include "Rte/Common.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"

#include <iostream>
#include <memory>
#include <vector>

void ClientApp::menuLoop() {
    // 1*1 red texture for the buttons
    const std::shared_ptr<Rte::Graphic::Texture> redTexture = m_graphicModule->createTexture();
    redTexture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});

    std::unique_ptr<Button> button = std::make_unique<Button>(m_ecs, m_graphicModule, redTexture, Rte::Vec2<float>(0, 0), Rte::Vec2<float>(200, 50), [&]() {
        std::cout << "Button clicked\n";
    });

    while (m_running) {
        button->update();
        m_graphicModule->update();
    }
}
