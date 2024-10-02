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
    const std::shared_ptr<Rte::Graphic::Texture> transparentTexture = m_graphicModule->createTexture();
    transparentTexture->loadFromMemory(std::vector<Rte::u8>{0, 0, 0, 0}.data(), {1, 1});

    std::array<std::unique_ptr<Button>, 3> buttons {
        std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, Rte::Vec2<float>(0, -80), Rte::Vec2<float>(200, 50), "Play", 30, [&]() {
            std::cout << "Play button clicked\n";
        }),
        std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, Rte::Vec2<float>(0, 0), Rte::Vec2<float>(200, 50), "Options", 30, [&]() {
            std::cout << "Options button clicked\n";
        }),
        std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, Rte::Vec2<float>(0, 80), Rte::Vec2<float>(200, 50), "Quit", 30, [&]() {
            m_running = false;
        })
    };

    while (m_running) {
        for (const auto& button : buttons)
            button->update();

        m_graphicModule->update();
    }
}
