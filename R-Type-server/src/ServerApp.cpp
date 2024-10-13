#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

ServerApp::ServerApp() {
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
    m_networkModuleServer->start(123456);

    // Allocs
    m_entities = std::make_shared<std::vector<Rte::Entity>>();
}

void ServerApp::run() {
    std::mutex mutex;

    // Sandbox creation
    constexpr Rte::Vec2<float> sandBoxScale = {8, 8};
    constexpr Rte::Vec2<Rte::u16> sandBoxSize = {240, 135};
    constexpr Rte::Vec2<float> sandBoxPosition = {0, 0};

    const Rte::Entity sandBoxEntity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(sandBoxEntity, Rte::BasicComponents::Transform{sandBoxPosition, sandBoxScale, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(sandBoxEntity, Rte::Physics::Components::Physics{.sandBox = m_physicsModule->createSandBox(sandBoxSize)});


    // Player creation event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::PLAYER_CREATED, [&](Rte::Event& event) {
        // New player creation
        const uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);
        std::cout << "Creating player " << playerId << std::endl;
        m_players.insert({playerId, std::make_unique<Player>(m_ecs, m_graphicModule, m_physicsModule, m_currentUid++)});

        // Add to entity list
        const Rte::Entity newPlayerEntity = m_players[playerId]->getEntity();
        m_entities->emplace_back(newPlayerEntity);

        // Load texture and add to new entities textures
        const std::shared_ptr<Rte::Graphic::Texture>& texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(newPlayerEntity).texture;
        const Rte::u8 *pixels = texture->getPixels();
        std::vector<Rte::u8> pixelsVector(pixels, pixels + texture->getSize().x * texture->getSize().y * 4);

        Rte::Network::PackedTexture packedTexture{};
        packedTexture.size = texture->getSize();
        packedTexture.pixels = pixelsVector;

        m_newEntitiesTextures[newPlayerEntity] = packedTexture;
    }));


    // Player destroyed event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::PLAYER_DELETED, [&](Rte::Event& event) {
        const uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);
        const Rte::Entity playerEntity = m_players.at(playerId)->getEntity();

        mutex.lock(); {
            const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(playerEntity);

            m_entities->erase(std::remove(m_entities->begin(), m_entities->end(), playerEntity), m_entities->end());
            m_ecs->destroyEntity(playerEntity);
            m_players.erase(playerId);

            for (auto& [playerId, player] : m_players)
                m_networkModuleServer->deletePlayer(uid, playerId);
            m_networkModuleServer->updateEntity(m_entities);
        }
        mutex.unlock();
    }));


    // Input event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::INPUT, [&](Rte::Event& event) {
        const Rte::Network::PackedInput& packedInput = event.getParameter<Rte::Network::PackedInput>(Rte::Network::Events::Params::INPUT);
        uint32_t playerId = event.getParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID);

        if (packedInput.moveLeft)
            m_players.at(playerId)->move({-20, 0});
        if (packedInput.moveRight)
            m_players.at(playerId)->move({20, 0});
        if (packedInput.fly)
            m_players.at(playerId)->move({0, -20});
        if (packedInput.shoot) {
            std::cout << "Player " << playerId << " is shooting" << std::endl;

            std::cout << "Player Pos " << m_players.at(playerId)->getPos().x << " " << m_players.at(playerId)->getPos().y << std::endl;
            std::cout << "Mouse Pos " << packedInput.mousePos.x - m_graphicModule->getWindowSize().x / 2.F << " " << packedInput.mousePos.y - m_graphicModule->getWindowSize().y / 2.F << std::endl;
            const float shootAngle = -std::atan2(
                static_cast<float>(m_players.at(playerId)->getPos().x) - (static_cast<float>(packedInput.mousePos.x) - m_graphicModule->getWindowSize().x / 2.F),
                static_cast<float>(m_players.at(playerId)->getPos().y) - (static_cast<float>(packedInput.mousePos.y) - m_graphicModule->getWindowSize().y / 2.F)
            ) - std::numbers::pi_v<float> / 2;

            Rte::Entity projectile = m_players.at(playerId)->shoot(shootAngle);
            if (projectile == 0)
                return;

            m_ecs->addComponent(projectile, Rte::BasicComponents::UidComponents{m_currentUid++});

            m_projectiles.push_back(std::make_unique<Rte::Entity>(projectile));
            m_entities->emplace_back(projectile);


            const std::shared_ptr<Rte::Graphic::Texture>& texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(projectile).texture;
            const Rte::u8 *pixels = texture->getPixels();
            std::vector<Rte::u8> pixelsVector(pixels, pixels + texture->getSize().x * texture->getSize().y * 4);

            Rte::Network::PackedTexture packedTexture{};
            packedTexture.size = texture->getSize();
            std::cout << "Texture size: " << packedTexture.size.x << " " << packedTexture.size.y << std::endl;
            packedTexture.pixels = pixelsVector;

            m_newEntitiesTextures[projectile] = packedTexture;
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
        m_graphicModule->update();

        for (auto& [playerId, player] : m_players)
            player->update();
        m_networkModuleServer->update();
        m_networkModuleServer->updateTexture(m_newEntitiesTextures);
        m_networkModuleServer->updateEntity(m_entities);
        m_networkModuleServer->sendUpdate();
    }
}
