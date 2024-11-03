#include "DemoApp.hpp"

#include "Rte/Audio/Sound.hpp"
#include "Rte/Audio/SoundBuffer.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Scripting/Components.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <memory>
#include <numbers>

DemoApp::DemoApp() {
    m_ecs = std::make_shared<Rte::Ecs>();


    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type-demo");
    m_graphicModule->setWindowSize({1280, 720});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);
    m_graphicModule->loadFontFromFile("../assets/alagard.ttf");
    m_graphicModule->setLayerCount(10);


    // Load the scripting module
    m_scriptingModule = Rte::interfaceCast<Rte::Scripting::ScriptingModule>(moduleManager.loadModule("RteScripting"));
    m_scriptingModule->init(m_ecs);
    m_scriptingModule->setGraphicModule(m_graphicModule);


    // Load the audio module
    m_audioModule = Rte::interfaceCast<Rte::Audio::AudioModule>(moduleManager.loadModule("RteAudio"));
    m_audioModule->init(m_ecs);


    // Event callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));
}

DemoApp::~DemoApp() {
}

void DemoApp::run() {
    std::shared_ptr<Rte::Audio::SoundBuffer> soundBuffer = m_audioModule->createSoundBuffer("../assets/clown.mp3");
    std::shared_ptr<Rte::Audio::Sound> sound = m_audioModule->createSound(soundBuffer);
    sound->play();

    const uint32_t projectileTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(projectileTexture, "../assets/jb.jpg"))
        throw std::runtime_error("Failed to load texture: ../assets/jb.jpg");

    const uint32_t playerTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(playerTexture, "../assets/qpn.jpg"))
        throw std::runtime_error("Failed to load texture: ../assets/qpn.jpg");

    const Rte::Entity player = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(player, Rte::BasicComponents::Transform{.position = {0, 0}, .scale = {0.3, 0.3}, .rotation = 0.F});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(player, Rte::Graphic::Components::Sprite{.textureId = playerTexture, .layer = 0});
    m_ecs->addComponent<Rte::Scripting::Components::Script>(player, Rte::Scripting::Components::Script{R"(
        function update()
            if isKeyPressed("Z") then
                Transform.position.y = Transform.position.y - deltaTime * 300
            end
            if isKeyPressed("S") then
                Transform.position.y = Transform.position.y + deltaTime * 300
            end
            if isKeyPressed("Q") then
                Transform.position.x = Transform.position.x - deltaTime * 300
            end
            if isKeyPressed("D") then
                Transform.position.x = Transform.position.x + deltaTime * 300
            end
        end

        update()
    )"});

    float timePassed = 0;
    while (m_running) {
        static std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
        const std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
        timePassed += std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        if (m_graphicModule->isMouseButtonPressed(Rte::Graphic::MouseButton::Left) && timePassed > 0.05) {
            timePassed = 0;

            const Projectile projectile {
                .entity = m_ecs->createEntity(),
                .distanceDone = 0
            };
            m_projectiles.push_back(projectile);

            const Rte::BasicComponents::Transform& playerTransform = m_ecs->getComponent<Rte::BasicComponents::Transform>(player);
            const Rte::Vec2<Rte::u16>& mousePosition = m_graphicModule->getMousePosition();
            const Rte::Vec2<float> mousePosCentered = {
                static_cast<float>(mousePosition.x - m_graphicModule->getWindowSize().x / 2),
                static_cast<float>(mousePosition.y - m_graphicModule->getWindowSize().y / 2)
            };
            const Rte::Vec2<float> playerMouseOffset = {
                mousePosCentered.x - playerTransform.position.x,
                mousePosCentered.y - playerTransform.position.y
            };
            const float angle = atan2(playerMouseOffset.y, playerMouseOffset.x) * 180 / std::numbers::pi + 90;

            m_ecs->addComponent<Rte::BasicComponents::Transform>(projectile.entity, Rte::BasicComponents::Transform{.position = playerTransform.position, .scale = {0.01, 0.01}, .rotation = angle});
            m_ecs->addComponent<Rte::Graphic::Components::Sprite>(projectile.entity, Rte::Graphic::Components::Sprite{.textureId = projectileTexture, .layer = 0});
            m_ecs->addComponent<Rte::Scripting::Components::Script>(projectile.entity, Rte::Scripting::Components::Script{R"(
                function update()
                    local angle = Transform.rotation - 90   -- Angle in degrees
                    local rad_angle = math.rad(angle)       -- Convert angle to radians

                    local vector = {math.cos(rad_angle), math.sin(rad_angle)}
                    Transform.position.x = Transform.position.x + vector[1] * deltaTime * 1000
                    Transform.position.y = Transform.position.y + vector[2] * deltaTime * 1000
                end

                function printPosition()
                    print(Transform.position.x, Transform.position.y)
                end

                update()
            )"});
        }

        for (int i = 0; i < m_projectiles.size(); i++) {
            const Projectile& projectile = m_projectiles[i];
            m_projectiles[i].distanceDone += 1;
            if (m_projectiles[i].distanceDone > 300) {
                m_ecs->destroyEntity(projectile.entity);
                m_projectiles.erase(m_projectiles.begin() + i);
                i--;
            }
        }

        m_graphicModule->update();
        m_scriptingModule->update();
    }
}
