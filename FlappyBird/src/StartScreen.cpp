#include "Button.hpp"
#include "FlappyBird.hpp"

void FlappyBird::titleScreen() {
    bool running = true;

    // Quentin Sprite
    const Rte::Entity quentin = createSpriteEntity("../assets/qpn.jpg", {0, -50}, {0.8, 0.8}, 0, 0);

    // Buttons
    uint32_t transparentTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(transparentTexture, std::vector<Rte::u8>{100, 100, 100, 200}.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");
    const std::unique_ptr<Button> startButton = std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, true, Rte::Vec2<float>(0, 100), Rte::Vec2<float>(150, 50), "Start", 30, [&]() {
        running = false;
    });

    const std::unique_ptr<Button> quitButton = std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, true, Rte::Vec2<float>(0, 200), Rte::Vec2<float>(150, 50), "Quit", 30, [&]() {
        m_running = false;
    });

    while (m_running && running) {

        m_graphicModule->drawText({-100, -200}, "Flappy Quentin", 30);

        m_graphicModule->update();
        startButton->update();
        quitButton->update();
    }

    m_ecs->destroyEntity(quentin);
}
