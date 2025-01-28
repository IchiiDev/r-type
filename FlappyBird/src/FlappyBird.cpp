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
