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


    // Creation of a 1*1 red texture
    const std::shared_ptr<Rte::Graphic::Texture> texture = graphicModule->createTexture();
    texture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});


    // Creation of a drawable entity
    constexpr Rte::Vec2<float> entityScale = {100, 100};
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
    
    //load the physics module
    const std::shared_ptr<Rte::Physics::PhysicsModule> physicsModule = Rte::interfaceCast<Rte::Physics::PhysicsModule>(moduleManager.loadModule("RtePhysics"));
    physicsModule->init(m_ecs);
    std::shared_ptr<Rte::Physics::RigidBody> rigidBody = physicsModule->createRigidBody(
        Rte::Physics::BodyType::DYNAMIC,
        std::vector<Rte::u8>{255, 0, 0, 255},
        1,
        0.3,
        entityPosition,
        entityScale
    );

    m_ecs->addComponent<Rte::Physics::Components::Physics>(entity, Rte::Physics::Components::Physics{rigidBody});

    // Ground entity

    Rte::Entity ground = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(ground, Rte::Graphic::Components::Sprite(texture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(ground, Rte::BasicComponents::Transform{
        .position = {(static_cast<float>(windowSize.x) / 2) - 550, (static_cast<float>(windowSize.y) / 2) + 400},
        .scale = {1000, 100},
        .rotation = 0
    });
    
    std::shared_ptr<Rte::Physics::RigidBody> groundBody = physicsModule->createRigidBody(
        Rte::Physics::BodyType::STATIC,
        std::vector<Rte::u8>{0, 255, 0, 255},
        1,
        0.3,
        {(static_cast<float>(windowSize.x) / 2) - 550, (static_cast<float>(windowSize.y) / 2) + 400},
        {1000, 100}
    );

    m_ecs->addComponent<Rte::Physics::Components::Physics>(ground, Rte::Physics::Components::Physics{groundBody});

    // Callback to close the window
    bool running = true;
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::Window::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            running = false;
        }
    ));


    // Callback to move the sprite and make it at the center of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::Window::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const float width = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::WIDTH);
            const float height = event.getParameter<Rte::u16>(Rte::Graphic::Events::Window::Resized::HEIGHT);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);
            transform.position.x = (width / 2) - (entityScale.x / 2);
            transform.position.y = (height / 2) - (entityScale.y / 2);
        }
    ));


    // Main loop
    while (running) {
        graphicModule->update();
        physicsModule->update();
    }
}
