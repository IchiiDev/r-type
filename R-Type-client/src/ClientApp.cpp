#include "ClientApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"

#include <chrono>
#include <cmath>
#include <memory>
#include <vector>

ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();


    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type");
    m_graphicModule->setWindowSize({1280, 720});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);


    // Creation of a 1*1 red texture
    m_redTexture = m_graphicModule->createTexture();
    m_redTexture->loadFromMemory(std::vector<Rte::u8>{255, 0, 0, 255}.data(), {1, 1});


    // Callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));


    // Callback to align the player with the size of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const Rte::Vec2<Rte::u16> newSize = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::NEW_WINDOW_SIZE);

            // Update the sprite position
            // Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_playerEntity);
            // playerTransform.position.y = static_cast<float>(newSize.y) - playerTransform.scale.y;
        }
    ));


    // Entities creation
    m_playerEntity = m_ecs->createEntity();     // NOLINT(cppcoreguidelines-prefer-member-initializer)
    m_sightLineEntity = m_ecs->createEntity();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
}

void ClientApp::run() {
    // Player configuration
    constexpr Rte::Vec2<float>  playerScale = {100, 100};
    const Rte::Vec2<Rte::u16>   windowSize = m_graphicModule->getWindowSize();
    const Rte::Vec2<float>      playerPosition = {0, static_cast<float>(m_graphicModule->getWindowSize().y) - playerScale.y};

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_playerEntity, Rte::Graphic::Components::Sprite(m_redTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_playerEntity, Rte::BasicComponents::Transform{
        .position = playerPosition,
        .scale = playerScale,
        .rotation = 0
    });


    // Sight line configuration
    constexpr Rte::Vec2<float> sightLineScale = {2, 500.0};

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_sightLineEntity, Rte::Graphic::Components::Sprite(m_redTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_sightLineEntity, Rte::BasicComponents::Transform{
        .position = {0, 0},
        .scale = sightLineScale,
        .rotation = 0
    });


    // Variables for jetpack jump
    bool isJumping = false;
    float verticalVelocity = 0.0;
    const float gravity = 980.0;
    const float jumpForce = 500.0;


    // Main loop
    while (m_running) {
        const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();


        // Update player position
        Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_playerEntity);
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::D))
            playerTransform.position.x += 500 * m_deltaTime;
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Q))
            playerTransform.position.x -= 500 * m_deltaTime;


        // Jetpack jump
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Space) && !isJumping) {
            isJumping = true;
            verticalVelocity = -jumpForce;
        }

        if (isJumping) {
            verticalVelocity += gravity * m_deltaTime;
            playerTransform.position.y += verticalVelocity * m_deltaTime;

            // Check if player has landed
            if (playerTransform.position.y >= static_cast<float>(windowSize.y) - playerTransform.scale.y) {
                playerTransform.position.y = static_cast<float>(windowSize.y) - playerTransform.scale.y;
                isJumping = false;
                verticalVelocity = 0.0;
            }
        }


        {   // Sight line update

            // Position update
            Rte::BasicComponents::Transform& sightLineTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(m_sightLineEntity);
            const Rte::Vec2<Rte::u16> mousePosition = m_graphicModule->getMousePosition();
            sightLineTransform.position = { playerTransform.position.x + playerTransform.scale.x / 2, playerTransform.position.y + playerTransform.scale.y / 2 };


            // Rotation update
            const float deltaX = static_cast<float>(mousePosition.x) - (playerTransform.position.x + playerTransform.scale.x / 2);
            const float deltaY = static_cast<float>(mousePosition.y) - (playerTransform.position.y + playerTransform.scale.y / 2);
            sightLineTransform.rotation = static_cast<float>(std::atan2(deltaY, deltaX) * 180.0 / static_cast<float>(M_PI) - 90.0);
        }


        // Module updates
        m_graphicModule->update();


        // Update delta time (as milliseconds)
        const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        m_deltaTime = std::chrono::duration<float>(end - start).count();
    }
}
