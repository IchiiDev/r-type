#include "Button.hpp"
#include "FlappyBird.hpp"
#include "Rte/Graphic/Components.hpp"

void FlappyBird::pauseScreen(Rte::Audio::Sound &music) {
    bool running = true;

    // Texture
    uint32_t transparentTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(transparentTexture, std::vector<Rte::u8>{100, 100, 100, 200}.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");

    const Rte::Entity pauseScreen = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(pauseScreen, Rte::BasicComponents::Transform{.position = {0, 0}, .scale = {300, 600}, .rotation = 0});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(pauseScreen, Rte::Graphic::Components::Sprite{.textureId = transparentTexture, .layer = 0});

    // Buttons
    uint32_t buttonTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(buttonTexture, std::vector<Rte::u8>{0, 0, 0, 255}.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");

    const std::unique_ptr<Button> resumeButton = std::make_unique<Button>(m_ecs, m_graphicModule, buttonTexture, true, Rte::Vec2<float>(0, -100), Rte::Vec2<float>(150, 50), "Resume", 30, [&]() {
        running = false;
    });

    const std::unique_ptr<Button> musicToggleButton = std::make_unique<Button>(m_ecs, m_graphicModule, buttonTexture, true, Rte::Vec2<float>(0, 0), Rte::Vec2<float>(150, 50), "Music", 30, [&]() {
        pauseMusic = !pauseMusic;
        if (pauseMusic)
            music.pause();
        else
            music.play();
    });

    const std::unique_ptr<Button> soundToggleButton = std::make_unique<Button>(m_ecs, m_graphicModule, buttonTexture, true, Rte::Vec2<float>(0, 100), Rte::Vec2<float>(150, 50), "Sound", 30, [&]() {
        pauseSound = !pauseSound;
    });

    while (running && m_running) {

        m_graphicModule->update();

        resumeButton->update();
        musicToggleButton->update();
        soundToggleButton->update();

        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::P)) {
            running = false;
        }

    }

    m_ecs->destroyEntity(pauseScreen);
}
