#include "ClientApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include <iostream>
#include <memory>
#include <vector>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

void ClientApp::run() {
    // Load the graphic module
    const std::shared_ptr<Rte::Graphic::GraphicModule> graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    graphicModule->init(m_ecs);
    graphicModule->setWindowTitle("R-Type");
    graphicModule->setWindowSize({1920, 1080});
    graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);

    //load the physics module
    const std::shared_ptr<Rte::Physics::PhysicsModule> physicsModule = Rte::interfaceCast<Rte::Physics::PhysicsModule>(moduleManager.loadModule("RtePhysics"));
    physicsModule->init(m_ecs);

    // Creation of a drawable entity
    // Creation of a texture
    const std::shared_ptr<Rte::Graphic::Texture> texture = graphicModule->createTexture();
    texture->loadFromFile("../sprite3.png");
    
    constexpr Rte::Vec2<float> entityScale = {8, 8};
    const Rte::Vec2<Rte::u16> windowSize = graphicModule->getWindowSize();
    const Rte::Vec2<float> entityPosition = {
        (static_cast<float>(windowSize.x) / 2) - (entityScale.x / 2),
        (static_cast<float>(windowSize.y) / 2) - (entityScale.y / 2)
    };
    Rte::Entity entity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite(texture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{
        .position = entityPosition,
        .scale = entityScale,
        .rotation = 0
    });
    
    std::shared_ptr<Rte::Physics::RigidBody> rigidBody = physicsModule->createRigidBody(
        Rte::Physics::BodyType::DYNAMIC,
        m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity).texture->getPixels(),
        m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity).texture->getSize(),
        1,
        0.3,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).position,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).scale,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).rotation
    );

    m_ecs->addComponent<Rte::Physics::Components::Physics>(entity, Rte::Physics::Components::Physics{rigidBody});

    //creation of a second entity
    const std::shared_ptr<Rte::Graphic::Texture> texture2 = graphicModule->createTexture();
    texture2->loadFromFile("../floor.png");

    std::cout << "Creating body" << std::endl;

    constexpr Rte::Vec2<float> entityScale2 = {8, 8};
    const Rte::Vec2<float> entityPosition2 = {
        (static_cast<float>(windowSize.x) / 2) - (entityScale.x / 2),
        (static_cast<float>(windowSize.y) / 2) - (entityScale.y / 2) + 400
    };
    Rte::Entity entity2 = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity2, Rte::Graphic::Components::Sprite(texture2));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(entity2, Rte::BasicComponents::Transform{
        .position = entityPosition2,
        .scale = entityScale2,
        .rotation = 0
    });

    std::shared_ptr<Rte::Physics::RigidBody> rigidBody2 = physicsModule->createRigidBody(
        Rte::Physics::BodyType::STATIC,
        m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity2).texture->getPixels(),
        m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity2).texture->getSize(),
        1,
        0.3,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity2).position,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity2).scale,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(entity2).rotation
    );

    m_ecs->addComponent<Rte::Physics::Components::Physics>(entity2, Rte::Physics::Components::Physics{rigidBody2});

    // Callback to close the window
    bool running = true;
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            running = false;
        }
    ));


    // Callback to move the sprite and make it at the center of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const Rte::Vec2<Rte::u16> newSize = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::NEW_WINDOW_SIZE);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
            transform.position.x = (static_cast<float>(newSize.x) / 2) - (entityScale.x / 2);
            transform.position.y = (static_cast<float>(newSize.y) / 2) - (entityScale.y / 2);
        }
    ));


    // Main loop
    int i = 0;
    while (running) {
        if (i % 3600 == -1) {
            Rte::Entity entity = m_ecs->createEntity();
            m_ecs->addComponent<Rte::Graphic::Components::Sprite>(entity, Rte::Graphic::Components::Sprite(texture));
            m_ecs->addComponent<Rte::BasicComponents::Transform>(entity, Rte::BasicComponents::Transform{
                .position = entityPosition,
                .scale = entityScale,
                .rotation = 0
            });
            std::shared_ptr<Rte::Physics::RigidBody> rigidBody = physicsModule->createRigidBody(
                Rte::Physics::BodyType::DYNAMIC,
                m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity).texture->getPixels(),
                m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity).texture->getSize(),
                1,
                0.3,
                m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).position,
                m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).scale,
                m_ecs->getComponent<Rte::BasicComponents::Transform>(entity).rotation
            );

            m_ecs->addComponent<Rte::Physics::Components::Physics>(entity, Rte::Physics::Components::Physics{rigidBody});
        }
        graphicModule->update();
        physicsModule->update();
        i++;
    }
}
