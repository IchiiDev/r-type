#include "ServerApp.hpp"

#include "Player.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
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
    // Sandbox creation
    constexpr Rte::Vec2<float> sandBoxScale = {8, 8};
    constexpr Rte::Vec2<Rte::u16> sandBoxSize = {240, 135};
    constexpr Rte::Vec2<float> sandBoxPosition = {0, 0};

    const Rte::Entity sandBoxEntity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(sandBoxEntity, Rte::BasicComponents::Transform{sandBoxPosition, sandBoxScale, 0});
    m_ecs->addComponent<Rte::Physics::Components::Physics>(sandBoxEntity, Rte::Physics::Components::Physics{.sandBox = m_physicsModule->createSandBox(sandBoxSize)});


    // Player creation event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::PLAYER_CREATED, [&](const Rte::Event& /* UNUSED */) {
        m_players.emplace_back(m_ecs, m_graphicModule, m_physicsModule, m_currentUid++);

        const Rte::Entity newPlayerEntity = m_players.back().getEntity();
        m_entities->emplace_back(newPlayerEntity);

        const std::shared_ptr<Rte::Graphic::Texture>& texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(newPlayerEntity).texture;
        const Rte::u8 *pixels = texture->getPixels();
        std::vector<Rte::u8> pixelsVector(pixels, pixels + texture->getSize().x * texture->getSize().y * 4);

        Rte::Network::PackedTexture packedTexture{};
        packedTexture.size = texture->getSize();
        packedTexture.pixels = pixelsVector;

        m_newEntitiesTextures[newPlayerEntity] = packedTexture;
    }));


    // Input event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::INPUT, [&](Rte::Event& event) {
        const Rte::Network::PackedInput& packedInput = event.getParameter<Rte::Network::PackedInput>(Rte::Network::Events::Params::INPUT);

        for (Player& player : m_players) {
            if (packedInput.moveLeft)
                player.move({-20, 0});
            if (packedInput.moveRight)
                player.move({20, 0});
            if (packedInput.fly)
                player.move({0, -20});

            // TODO: handle shoot
        }
    }));


    // Player disconnected
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::DISCONNECTED, [&](const Rte::Event& event) {
        for (auto entity : *m_entities)
            m_ecs->destroyEntity(entity);
        m_entities->clear();
        m_players.clear();
    }));


    // Main loop
    while (true) {
        m_physicsModule->update();

        m_networkModuleServer->updateTexture(m_newEntitiesTextures);
        m_networkModuleServer->updateEntity(m_entities);
        m_networkModuleServer->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
