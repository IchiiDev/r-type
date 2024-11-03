#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"

#include <chrono>
#include <cmath>
#include <numbers>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

using KeyCharacter = struct {
    Rte::Graphic::Key key;
    char character;
};
using KeyCharacterArray = std::vector<KeyCharacter>;
const KeyCharacterArray CHARS = {
    {Rte::Graphic::Key::A, 'a'},
    {Rte::Graphic::Key::B, 'b'},
    {Rte::Graphic::Key::C, 'c'},
    {Rte::Graphic::Key::D, 'd'},
    {Rte::Graphic::Key::E, 'e'},
    {Rte::Graphic::Key::F, 'f'},
    {Rte::Graphic::Key::G, 'g'},
    {Rte::Graphic::Key::H, 'h'},
    {Rte::Graphic::Key::I, 'i'},
    {Rte::Graphic::Key::J, 'j'},
    {Rte::Graphic::Key::K, 'k'},
    {Rte::Graphic::Key::L, 'l'},
    {Rte::Graphic::Key::M, 'm'},
    {Rte::Graphic::Key::N, 'n'},
    {Rte::Graphic::Key::O, 'o'},
    {Rte::Graphic::Key::P, 'p'},
    {Rte::Graphic::Key::Q, 'q'},
    {Rte::Graphic::Key::R, 'r'},
    {Rte::Graphic::Key::S, 's'},
    {Rte::Graphic::Key::T, 't'},
    {Rte::Graphic::Key::U, 'u'},
    {Rte::Graphic::Key::V, 'v'},
    {Rte::Graphic::Key::W, 'w'},
    {Rte::Graphic::Key::X, 'x'},
    {Rte::Graphic::Key::Y, 'y'},
    {Rte::Graphic::Key::Z, 'z'},
    {Rte::Graphic::Key::Num0, '0'},
    {Rte::Graphic::Key::Num1, '1'},
    {Rte::Graphic::Key::Num2, '2'},
    {Rte::Graphic::Key::Num3, '3'},
    {Rte::Graphic::Key::Num4, '4'},
    {Rte::Graphic::Key::Num5, '5'},
    {Rte::Graphic::Key::Num6, '6'},
    {Rte::Graphic::Key::Num7, '7'},
    {Rte::Graphic::Key::Num8, '8'},
    {Rte::Graphic::Key::Num9, '9'},
    {Rte::Graphic::Key::Space, ' '},
    {Rte::Graphic::Key::Comma, ','},
    {Rte::Graphic::Key::Period, '.'},
    {Rte::Graphic::Key::Semicolon, ';'},
    {Rte::Graphic::Key::Apostrophe, '\''},
    {Rte::Graphic::Key::Grave, '`'},
    {Rte::Graphic::Key::Equal, '='},
    {Rte::Graphic::Key::Hyphen, '-'},
    {Rte::Graphic::Key::Slash, '/'},
    {Rte::Graphic::Key::Backslash, '\\'},
    {Rte::Graphic::Key::LBracket, '{'},
    {Rte::Graphic::Key::RBracket, '}'}
};

ServerApp::ServerApp() : m_rightWall(0), m_leftWall(0), m_topWall(0), m_bottomWall(0) {
    m_ecs = std::make_shared<Rte::Ecs>();

    // Load the physics module
    m_physicsModule = Rte::interfaceCast<Rte::Physics::PhysicsModule>(moduleManager.loadModule("RtePhysics"));
    m_physicsModule->init(m_ecs);

    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type");
    m_graphicModule->setWindowSize({1920, 1080});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);
    m_graphicModule->loadFontFromFile("../assets/alagard.ttf");
    m_graphicModule->setLayerCount(10);

    // Load the network module
    m_networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    m_networkModuleServer = m_networkModule->getServer();
    m_networkModuleServer->init(m_ecs);
    m_networkModuleServer->start(12345);

    // Allocs
    m_entities = std::make_shared<std::vector<Rte::Entity>>();

    // Time init
    m_enemyClock = std::chrono::high_resolution_clock::now();
    m_startTime = std::chrono::high_resolution_clock::now();
}

void ServerApp::run() {

    // Sandbox creation
    constexpr Rte::Vec2<float> sandBoxScale = {8, 8};
    constexpr Rte::Vec2<Rte::u16> sandBoxSize = {240, 135};
    constexpr Rte::Vec2<float> sandBoxPosition = {0, 0};

    const Rte::Entity sandBoxEntity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(sandBoxEntity, Rte::BasicComponents::Transform{sandBoxPosition, sandBoxScale, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(sandBoxEntity, Rte::Physics::Components::Physics{.sandBox = m_physicsModule->createSandBox(sandBoxSize)});

    // Init scene
    initScene();

    // Semi transparent texture
    m_devConsoleTexture = m_graphicModule->createTexture();
    std::vector<Rte::u8> semiTransparentPixel = {0, 0, 0, 200};

    if (!m_graphicModule->loadTextureFromMemory(m_devConsoleTexture, semiTransparentPixel.data(), {1, 1}))
        throw std::runtime_error("Failed to create texture from memory");

    // Player creation event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::PLAYER_CREATED, [&](Rte::Event& event) {
        // New player creation
        const uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);
        m_players.insert({playerId, std::make_unique<Player>(m_ecs, m_graphicModule, m_physicsModule, m_currentUid++)});

        // Add to entity list
        const Rte::Entity newPlayerEntity = m_players[playerId]->getEntity();
        m_entities->emplace_back(newPlayerEntity);

        // Load texture and add to new entities textures
        uint32_t texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(newPlayerEntity).textureId;
        const uint8_t* texturePixels = m_graphicModule->getTexturePixels(texture);
        const Rte::Vec2<Rte::u16> textureSize = m_graphicModule->getTextureSize(texture);

        std::vector<Rte::u8> pixelsVector(texturePixels, texturePixels + static_cast<ptrdiff_t>(textureSize.x * textureSize.y) * 4);

        Rte::Network::PackedTexture packedTexture{};
        packedTexture.size = textureSize;
        packedTexture.pixels = pixelsVector;

        m_newEntitiesTextures[newPlayerEntity] = packedTexture;
    }));


    // Player destroyed event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::PLAYER_DELETED, [&](Rte::Event& event) {
        const uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);
        const Rte::Entity playerEntity = m_players.at(playerId)->getEntity();

        const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(playerEntity);

        m_entities->erase(std::remove(m_entities->begin(), m_entities->end(), playerEntity), m_entities->end());
        m_ecs->destroyEntity(playerEntity);
        m_players.erase(playerId);

        for (auto& [playerId, player] : m_players)
            m_networkModuleServer->deletePlayer(uid, playerId);
        m_networkModuleServer->updateEntity(m_entities);
    }));


    // Input event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::INPUT, [&](Rte::Event& event) {
        const Rte::Network::PackedInput& packedInput = event.getParameter<Rte::Network::PackedInput>(Rte::Network::Events::Params::INPUT);
        uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);

        if (packedInput.moveUp)
            m_players.at(playerId)->fly({0, 3});

        if (packedInput.moveLeft)
            m_players.at(playerId)->move({-10, 0});
        else if (packedInput.moveRight)
            m_players.at(playerId)->move({10, 0});
        else {
            m_players.at(playerId)->move({0, 0});
        }

        if (packedInput.shoot) {
            const float shootAngle = -std::atan2(
                static_cast<float>(m_players.at(playerId)->getPos().x) - (static_cast<float>(packedInput.shootDirection.x) - m_graphicModule->getWindowSize().x / 2.F),
                static_cast<float>(m_players.at(playerId)->getPos().y) - (static_cast<float>(packedInput.shootDirection.y) - m_graphicModule->getWindowSize().y / 2.F)
            ) - std::numbers::pi_v<float> / 2;
            Rte::Entity projectile = m_players.at(playerId)->shoot(shootAngle);
            if (projectile != 0)
                createProjectile(projectile);
        }
    }));

    // Dev console event listener
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::KEY_PRESSED, [&](Rte::Event& event) {
        Rte::Graphic::Key key = event.getParameter<Rte::Graphic::Key>(Rte::Graphic::Events::Params::KEY_PRESSED);

        if (key == Rte::Graphic::Key::F12) {
            m_showDevConsole = !m_showDevConsole;
        }
        if (m_showDevConsole) {

            // Handle special inputs
            if (key == Rte::Graphic::Key::Backspace) {
                if (!m_devConsoleInput.empty())
                    m_devConsoleInput.pop_back();
            }
            if (key == Rte::Graphic::Key::Enter) {
                if (!m_devConsoleInput.empty()) {
                    this->handleConsoleInput();
                    m_devConsoleInput.clear();
                }
            }

            // Handle text input
            for (const KeyCharacter& keyChar : CHARS) {
                if (keyChar.key == key) {
                    m_devConsoleInput += keyChar.character;
                    break;
                }
            }
        }
    }));



    // Player disconnected
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::DISCONNECTED, [&](const Rte::Event& /* UNUSED */) {
        for (auto entity : *m_entities)
            m_ecs->destroyEntity(entity);
        m_entities->clear();
        m_players.clear();
    }));


    // Exit event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT, [&](const Rte::Event& /* UNUSED */) {
        m_running = false;
    }));


    // Main loop
    while (m_running) {
        m_physicsModule->update();
        if (m_showDevConsole)
            drawDevConsole();
        m_graphicModule->update();

        updateScene();
        m_networkModuleServer->update();
        m_networkModuleServer->updateTexture(m_newEntitiesTextures);
        m_networkModuleServer->updateEntity(m_entities);
        m_networkModuleServer->sendUpdate();
    }
}

void ServerApp::handleConsoleInput() {
    m_devConsoleHistory.emplace_back(">> " + m_devConsoleInput);
    if (m_devConsoleInput == "exit") {
        m_running = false;
    }
    else if (m_devConsoleInput == "clear") {
        m_devConsoleHistory.clear();
    }
    else if (m_devConsoleInput == "uwu") {

        m_devConsoleHistory.emplace_back("     _");
        m_devConsoleHistory.emplace_back("   _| |");
        m_devConsoleHistory.emplace_back(" _| | |");
        m_devConsoleHistory.emplace_back("| | | |");
        m_devConsoleHistory.emplace_back("| | | | __");
        m_devConsoleHistory.emplace_back("| | | |/  \\");
        m_devConsoleHistory.emplace_back("|       /\\ \\");
        m_devConsoleHistory.emplace_back("|      /  \\/");
        m_devConsoleHistory.emplace_back("|      \\  /\\");
        m_devConsoleHistory.emplace_back("|       \\/ /");
        m_devConsoleHistory.emplace_back(" \\        /");
        m_devConsoleHistory.emplace_back("  |     /");
        m_devConsoleHistory.emplace_back("  |    /");

    }
    else {
        m_devConsoleHistory.emplace_back("Unknown command :( ");
    }
}

void ServerApp::drawDevConsole() {
    m_graphicModule->drawRectangle({0, 0}, {1920, 540}, m_devConsoleTexture);
    m_graphicModule->drawText({-960, 270}, ">> " + m_devConsoleInput, 30);

    // Draw history starting from the bottom up
    for (int i = 0; i < static_cast<int>(m_devConsoleHistory.size()); i++) {
        m_graphicModule->drawText({-960, 270 - (i + 1) * 30}, m_devConsoleHistory[m_devConsoleHistory.size() - i - 1], 30);
    }
}
