#include "ClientApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();

    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type");
    m_graphicModule->setWindowSize({1920, 1080});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);
    m_graphicModule->loadFontFromFile("../assets/alagard.ttf");
    m_graphicModule->setLayerCount(10);

    // Load the network module
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    m_networkModuleClient = networkModule->getClient();
    m_networkModuleClient->init(m_ecs);
    m_networkModuleClient->connect("127.0.0.1", 4242);

    // Event callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));

}

void ClientApp::run() {
    // Disconnect event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::DISCONNECTED, [&](const Rte::Event& /* UNUSED */) {
        m_running = false;
    }));


    // Entity created event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::ENTITY_CREATED, [&](Rte::Event& event) {
        m_entitiesMutex.lock(); {
            const Rte::Network::PackedNewEntity& packedNewEntity = event.getParameter<Rte::Network::PackedNewEntity>(Rte::Network::Events::Params::PACKED_NEW_ENTITY);
            if (std::find_if(m_entities.begin(), m_entities.end(), [&](const Rte::Entity& entity) {
                return m_ecs->getComponent<Rte::BasicComponents::UidComponents>(entity).uid == packedNewEntity.id;
            }) != m_entities.end()) {
                m_entitiesMutex.unlock();
                return;
            }

            std::shared_ptr<Rte::Graphic::Texture> newEntityTexture = m_graphicModule->createTexture();
            newEntityTexture->loadFromMemory(packedNewEntity.pixels.data(), packedNewEntity.size);

            const Rte::Entity newEntity = m_ecs->createEntity();
            m_entities.emplace_back(newEntity);
            m_ecs->addComponent<Rte::BasicComponents::Transform>(newEntity, packedNewEntity.transform);
            m_ecs->addComponent<Rte::Graphic::Components::Sprite>(newEntity, {newEntityTexture, 0});
            m_ecs->addComponent<Rte::BasicComponents::UidComponents>(newEntity, {packedNewEntity.id});

            m_entities.emplace_back(newEntity);
        }
        m_entitiesMutex.unlock();
    }));


    // Entity destroyed event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::ENTITY_DELETED, [&](Rte::Event& event) {
        const Rte::BasicComponents::UidComponents destroyedEntityUid = event.getParameter<Rte::BasicComponents::UidComponents>(Rte::Network::Events::Params::ENTITY_ID);

        m_entitiesMutex.lock(); {
            // Search for the entity with the given uid and destroy it
            for (const Rte::Entity& entity : m_entities) {
                if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>(entity).uid == destroyedEntityUid.uid) {
                    m_ecs->destroyEntity(entity);
                    m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
                    break;
                }
            }
        }
        m_entitiesMutex.unlock();
    }));


    // Entity updated event
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Network::Events::ENTITY_UPDATED, [&](Rte::Event& event) {
        const Rte::Network::PackedUpdateEntity& packedEntityUpdate = event.getParameter<Rte::Network::PackedUpdateEntity>(Rte::Network::Events::Params::PACKED_UPDATE_ENTITY);

        // Search for the entity with the given uid and update it
        for (const Rte::Entity& entity : m_entities) {
            if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>(entity).uid == packedEntityUpdate.id) {
                Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
                transform.position = packedEntityUpdate.transform.position;
                transform.rotation = packedEntityUpdate.transform.rotation;
                transform.scale = packedEntityUpdate.transform.scale;

                break;
            }
        }
    }));


    // Network thread
    std::thread networkThread([&] {
        while (m_running)
            m_networkModuleClient->update();
    });

    float shootAngle = 0;
    // Main loop
    while(m_running) {
        // Get inputs from player
        m_networkModuleClient->updateInputs(Rte::Network::PackedInput{
            .moveUp = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Z),
            .moveDown = m_graphicModule->isKeyPressed(Rte::Graphic::Key::S),
            .moveLeft = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Q),
            .moveRight = m_graphicModule->isKeyPressed(Rte::Graphic::Key::D),
            .shoot = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Space)
        });

        m_entitiesMutex.lock(); {
            m_graphicModule->update();
            m_networkModuleClient->sendUpdate();
        }
        m_entitiesMutex.unlock();
    }

    networkThread.join();
}
