#include "Button.hpp"
#include "FlappyBird.hpp"
#include <fstream>

void FlappyBird::endScreen() {
    if (score > highScore) {
        highScore = score;
        std::ofstream ofs("./save");
        ofs << highScore;
        ofs.close();
    }

    bool running = true;

    // Sound
    const std::shared_ptr<Rte::Audio::SoundBuffer> gameOverBuffer = m_audioModule->createSoundBuffer("../assets/fail.mp3");
    const std::shared_ptr<Rte::Audio::Sound> gameOverSound = m_audioModule->createSound(gameOverBuffer);
    if (!pauseSound)
        gameOverSound->play();

    // Buttons
    uint32_t transparentTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(transparentTexture, std::vector<Rte::u8>{100, 100, 100, 200}.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");
    const std::unique_ptr<Button> startButton = std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, true, Rte::Vec2<float>(0, 100), Rte::Vec2<float>(150, 50), "Play Again", 30, [&]() {
        running = false;
    });

    const std::unique_ptr<Button> quitButton = std::make_unique<Button>(m_ecs, m_graphicModule, transparentTexture, true, Rte::Vec2<float>(0, 200), Rte::Vec2<float>(150, 50), "Quit", 30, [&]() {
        m_running = false;
    });

    while (m_running && running) {
        m_graphicModule->drawText({-250, 0}, "Game Over", 30);
        m_graphicModule->drawText({-250, -50}, "Score: " + std::to_string(score), 30);
        m_graphicModule->drawText({-250, -100}, "High Score: " + std::to_string(highScore), 30);

        m_graphicModule->update();
        startButton->update();
        quitButton->update();
    }
    score = 0;
}
