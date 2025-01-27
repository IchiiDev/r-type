/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** FlappyBird
*/

#include "FlappyBird.hpp"
#include "Rte/Audio/Sound.hpp"
#include "Rte/Audio/SoundBuffer.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Button.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <tuple>
#include <fstream>

FlappyBird::FlappyBird() {
    m_ecs = std::make_shared<Rte::Ecs>();

    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("FlappyQuentin");

    m_graphicModule->setWindowSize({550, 800});
    m_graphicModule->loadFontFromFile("../assets/alagard.ttf");
    m_graphicModule->setLayerCount(10);

    // Set window position
    m_graphicModule->setWindowPosition({700, 100});

    // Load the Audio module
    m_audioModule = Rte::interfaceCast<Rte::Audio::AudioModule>(moduleManager.loadModule("RteAudio"));

    // Event callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));
}

FlappyBird::~FlappyBird() {}

void FlappyBird::gravity() {
    Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(player);

    playerTransform.position.y += g;
    g += 0.2;
    playerTransform.rotation = g * 5;
}

bool FlappyBird::playerPipeCollide(const Rte::Entity &player, const Rte::Entity &pipe) {
    Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(player);

    Rte::BasicComponents::Transform& pipeTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(pipe);

    Rect playerRect = {
        playerTransform.position.x - 50,
        playerTransform.position.y - 50,
        playerTransform.position.x + 50,
        playerTransform.position.y + 50
    };

    Rect pipeRect = {
        pipeTransform.position.x - 50,
        pipeTransform.position.y - 300,
        pipeTransform.position.x + 50,
        pipeTransform.position.y + 300
    };

    return (playerRect.x1 < pipeRect.x2 && playerRect.x2 > pipeRect.x1 && playerRect.y1 < pipeRect.y2 && playerRect.y2 > pipeRect.y1);

}

bool FlappyBird::playerScore(const Rte::Entity &player, std::tuple<Rect, Rte::Entity, bool> &pipe) {
    Rect pipeRect = std::get<0>(pipe);
    Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(player);

    if (playerTransform.position.x > pipeRect.x1 && !std::get<2>(pipe)) {
        std::get<2>(pipe) = true;
        return true;
    }
    return false;
}

void FlappyBird::gameLoop() {
    const uint32_t playerTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(playerTexture, "../assets/qpn.jpg"))
            throw std::runtime_error("Failed to load texture: ../assets/qpn.jpg");

    player = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(player, Rte::BasicComponents::Transform{.position = {-100, 0}, .scale = {0.3, 0.3}, .rotation = 0});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(player, Rte::Graphic::Components::Sprite{.textureId = playerTexture, .layer = 0});

    const uint32_t pipeTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(pipeTexture, "../assets/antoine_a_fail_tous_ses_projets.jpg"))
            throw std::runtime_error("Failed to load texture: ../assets/antoine_a_fail_tous_ses_projets.jpg");

    // Create a Vector of Tuples of Entities for the top and bottom pipes
    std::vector<std::tuple<Rte::Entity, Rte::Entity>> pipes;
    std::vector<std::tuple<Rect, Rte::Entity, bool>> scorePipes;

    for (int i = 0; i < 4; i++) {

        Rte::Entity topPipe = m_ecs->createEntity();
        Rte::Entity bottomPipe = m_ecs->createEntity();

        m_ecs->addComponent<Rte::BasicComponents::Transform>(topPipe, Rte::BasicComponents::Transform{.position = {static_cast<float>(300 + i * 400), -450}, .scale = {0.2, 0.3}, .rotation = 0});
        m_ecs->addComponent<Rte::Graphic::Components::Sprite>(topPipe, Rte::Graphic::Components::Sprite{.textureId = pipeTexture, .layer = 0});
        m_ecs->addComponent<Rte::BasicComponents::Transform>(bottomPipe, Rte::BasicComponents::Transform{.position = {static_cast<float>(300 + i * 400), 450}, .scale = {0.2, 0.3}, .rotation = 0});
        m_ecs->addComponent<Rte::Graphic::Components::Sprite>(bottomPipe, Rte::Graphic::Components::Sprite{.textureId = pipeTexture, .layer = 0});

        pipes.emplace_back(std::make_tuple(topPipe, bottomPipe));
        // Single 5 unit wide rect from top to bottom of the screen at the middle of the pipe

        Rect rect = {
            .x1 = static_cast<float>(300 + i * 400),
            .y1 = -450,
            .x2 = static_cast<float>(300 + i * 400) + 5,
            .y2 = 450
        };

        // Create a sprite for the score pipe
        const uint32_t scorePipeTexture = m_graphicModule->createTexture();
        std::vector<Rte::u8> semiTransparentPixel = {0, 0, 0, 0}; // Turn the pixel to 200gamma to make it semi-transparent
        if (!m_graphicModule->loadTextureFromMemory(scorePipeTexture, semiTransparentPixel.data(), {1, 1}))
            throw std::runtime_error("Failed to create texture from memory");

        Rte::Entity scorePipe = m_ecs->createEntity();

        m_ecs->addComponent<Rte::BasicComponents::Transform>(scorePipe, Rte::BasicComponents::Transform{.position = {rect.x1, 0}, .scale = {5, 900}, .rotation = 0});

        m_ecs->addComponent<Rte::Graphic::Components::Sprite>(scorePipe, Rte::Graphic::Components::Sprite{.textureId = scorePipeTexture, .layer = 0});

        scorePipes.emplace_back(std::make_tuple(rect, scorePipe, false));
    }

    const uint32_t backgroudTexture = m_graphicModule->createTexture();

    if (!m_graphicModule->loadTextureFromFile(backgroudTexture, "../assets/ethlar.png"))
            throw std::runtime_error("Failed to load texture: ../assets/ethlar.png");

    const Rte::Entity background = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(background, Rte::BasicComponents::Transform{.position = {0, 0}, .scale = {0.3, 0.3}, .rotation = 0});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(background, Rte::Graphic::Components::Sprite{.textureId = backgroudTexture, .layer = 1});

    const uint32_t scoreBoxTexture = m_graphicModule->createTexture();
    std::vector<Rte::u8> semiTransparentPixel = {0, 0, 0, 200};

    if (!m_graphicModule->loadTextureFromMemory(scoreBoxTexture, semiTransparentPixel.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");

    // Sounds and Music
    const std::shared_ptr<Rte::Audio::SoundBuffer> musicBuffer = m_audioModule->createSoundBuffer("../assets/zubzab.mp3");
    const std::shared_ptr<Rte::Audio::Sound> music = m_audioModule->createSound(musicBuffer);
    music->play();
    music->loop(true);

    if (pauseMusic)
        music->pause();

    const std::shared_ptr<Rte::Audio::SoundBuffer> jumpBuffer = m_audioModule->createSoundBuffer("../assets/jump.mp3");
    const std::shared_ptr<Rte::Audio::Sound> jumpSound = m_audioModule->createSound(jumpBuffer);

    float timePassed = 0;
    bool running = true;
    while (m_running && running) {

        // Handle game clock
        std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();

        const std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();

        timePassed += std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // REST OF THE GAME LOGIC

        // Move all the pipes left by 1 unit
        for (auto& pipe : pipes) {
            Rte::Entity topPipe = std::get<0>(pipe);
            Rte::Entity bottomPipe = std::get<1>(pipe);

            Rte::BasicComponents::Transform& topPipeTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(topPipe);
            Rte::BasicComponents::Transform& bottomPipeTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(bottomPipe);

            topPipeTransform.position.x -= 3;
            bottomPipeTransform.position.x -= 3;

            if (topPipeTransform.position.x < -600) {
                int offset = std::rand() % 200 - 100;
                int range = std::rand() % 50;

                topPipeTransform.position.x = 975;
                topPipeTransform.position.y = -450 + offset;
                bottomPipeTransform.position.x = 975;
                bottomPipeTransform.position.y = 450 + offset - range;
            }

        }

        // Move the score pipes left by 1 unit
        for (auto& pipeTuple : scorePipes) {
            Rect& pipe = std::get<0>(pipeTuple);
            Rte::Entity scorePipe = std::get<1>(pipeTuple);

            Rte::BasicComponents::Transform& scorePipeTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(scorePipe);
            pipe.x1 -= 3;
            pipe.x2 -= 3;

            scorePipeTransform.position.x -= 3;
            scorePipeTransform.position.y = 0;

            if (pipe.x1 < -600) {
                pipe.x1 = 975;
                pipe.x2 = 975;

                scorePipeTransform.position.x = 975;
                // Reset the score pipe
                std::get<2>(pipeTuple) = false;
            }
        }

        // If pipe passed the player, increment the score
        for (auto& pipeTuple : scorePipes) {
            if (playerScore(player, pipeTuple))
                score++;
        }

        // Draw Score Text

        m_graphicModule->drawRectangle({-250, 400}, {1100, 100}, scoreBoxTexture);
        m_graphicModule->drawText({-250, 350}, "Score: " + std::to_string(score), 30);
        m_graphicModule->drawText({50, 350}, "High Score: " + std::to_string(highScore), 30);

        // Check if the player collides with any of the pipes
        for (auto& pipe : pipes) {
            const Rte::Entity topPipe = std::get<0>(pipe);
            const Rte::Entity bottomPipe = std::get<1>(pipe);

            if (playerPipeCollide(player, topPipe) || playerPipeCollide(player, bottomPipe))
                running = false;

        }

        // Check out of bounds
        Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(player);
        if (playerTransform.position.y > 400 || playerTransform.position.y < -400)
            running = false;

        // If the player is pressing the space bar, move the player up
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Space) || m_graphicModule->isMouseButtonPressed(Rte::Graphic::MouseButton::Left)) {
            g = -5;
            if (!pauseSound)
                jumpSound->play();
        }
        gravity();

        // END OF THE GAME LOGIC

        // Pause menu
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::P) ||
            m_graphicModule->isKeyPressed(Rte::Graphic::Key::Escape)) {
            pauseScreen(*music);
        }

        // Update the graphic module
        m_graphicModule->update();

    }

    m_ecs->destroyEntity(player);
    for (auto& pipe : pipes) {
        m_ecs->destroyEntity(std::get<0>(pipe));
        m_ecs->destroyEntity(std::get<1>(pipe));
    }
    for (auto& pipeTuple : scorePipes) {
        m_ecs->destroyEntity(std::get<1>(pipeTuple));
    }
    m_ecs->destroyEntity(background);
    if (!m_running)
        return;
    music->stop();
    endScreen();
}

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

void FlappyBird::titleScreen() {
    bool running = true;

    // Quentin Sprite
    const uint32_t quentinTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(quentinTexture, "../assets/qpn.jpg"))
            throw std::runtime_error("Failed to load texture: ../assets/qpn.jpg");
    const Rte::Entity quentin = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(quentin, Rte::BasicComponents::Transform{.position = {0, -50}, .scale = {0.8, 0.8}, .rotation = 0});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(quentin, Rte::Graphic::Components::Sprite{.textureId = quentinTexture, .layer = 0});

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

void FlappyBird::run() {
    std::ifstream ifs("./save");
    if (ifs.good()) {
        ifs >> highScore;
        ifs.close();
    }

    // RNG Seed
    std::srand(std::time(nullptr));
    titleScreen();
    if (!m_running)
        return;
    while (m_running) {
        gameLoop();
    }

}
