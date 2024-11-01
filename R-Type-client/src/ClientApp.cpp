#include "ClientApp.hpp"

#include "Rte/Audio/Sound.hpp"
#include "Rte/Audio/SoundBuffer.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <thread>
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

    // Load the audio module
    m_audioModule = Rte::interfaceCast<Rte::Audio::AudioModule>(moduleManager.loadModule("RteAudio"));
    m_audioModule->init(m_ecs);

    // Load the network module
    const std::shared_ptr<Rte::Network::NetworkModule> networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    m_networkModuleClient = networkModule->getClient();
    m_networkModuleClient->init(m_ecs);
    m_networkModuleClient->connect("127.0.0.1", 123456);

    // Event callback to close window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::QUIT,
        [&](const Rte::Event& /* UNUSED */) {
            m_running = false;
        }
    ));

}

void ClientApp::run() {
    menuLoop();

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

            uint32_t newEntityTexture = m_graphicModule->createTexture();
            if (!m_graphicModule->loadTextureFromMemory(newEntityTexture, packedNewEntity.pixels.data(), packedNewEntity.size))
                throw std::runtime_error("Failed to load texture from memory");

            const Rte::Entity newEntity = m_ecs->createEntity();
            m_entities.emplace_back(newEntity);
            m_ecs->addComponent<Rte::BasicComponents::Transform>(newEntity, packedNewEntity.transform);
            m_ecs->addComponent<Rte::Graphic::Components::Sprite>(newEntity, {.textureId = newEntityTexture, .offset = {0, 0}, .layer = 0});
            m_ecs->addComponent<Rte::BasicComponents::UidComponents>(newEntity, {packedNewEntity.id});

            m_entities.emplace_back(newEntity);
        }
        m_entitiesMutex.unlock();
    }));

    // Create Dev Console
    m_devConsoleEntity = m_ecs->createEntity();
    uint32_t devConsoleTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(devConsoleTexture, "../assets/devConsole.png"))
        throw std::runtime_error("Failed to load texture: \"../assets/devConsole.png\"");
    m_ecs->addComponent<Rte::BasicComponents::Transform>(m_devConsoleEntity, Rte::BasicComponents::Transform{
        .position = {0, 0},
        .scale = {1, 1},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(m_devConsoleEntity, {.textureId = devConsoleTexture, .offset = {0, 0}, .layer = 1});

    // Create sky
    Rte::Entity sky = m_ecs->createEntity();
    uint32_t skytexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(skytexture, "../assets/sky.png"))
        throw std::runtime_error("Failed to load texture: \"../assets/sky.png\"");

    m_ecs->addComponent<Rte::BasicComponents::Transform>(sky, Rte::BasicComponents::Transform{
        .position = {0, 0},
        .scale = {10, 10},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(sky, {.textureId = skytexture, .offset = {0, 0}, .layer = 0});

    Rte::Entity sky1 = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(sky1, Rte::BasicComponents::Transform{
        .position = {1920, 0},
        .scale = {10, 10},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(sky1, {.textureId = skytexture, .offset = {0, 0}, .layer = 0});

    // Load audio module
    const std::shared_ptr<Rte::Audio::AudioModule> audioModule = Rte::interfaceCast<Rte::Audio::AudioModule>(moduleManager.loadModule("RteAudio"));
    audioModule->init(m_ecs);
    audioModule->update();

    const std::shared_ptr<Rte::Audio::SoundBuffer> soundBuffer = audioModule->createSoundBuffer("../assets/zubzab.mp3");
    const std::shared_ptr<Rte::Audio::Sound> sound = audioModule->createSound(soundBuffer);
    sound->play();

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

    // Dev console event listener
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::KEY_PRESSED, [&](Rte::Event& event) {
        Rte::Graphic::Key key = event.getParameter<Rte::Graphic::Key>(Rte::Graphic::Events::Params::KEY_PRESSED);

        if (key == Rte::Graphic::Key::F12) {
            m_showDevConsole = !m_showDevConsole;
        }
        if (!m_showDevConsole) {

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

    // Main loop
    while(m_running) {
        // Get inputs from player (disabled if dev console is open)
        if (!m_showDevConsole)
            m_networkModuleClient->updateInputs(Rte::Network::PackedInput{
                .moveUp = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Up),
                .moveDown = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Down),
                .moveLeft = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Left),
                .moveRight = m_graphicModule->isKeyPressed(Rte::Graphic::Key::Right),
                .shoot = m_graphicModule->isMouseButtonPressed(Rte::Graphic::MouseButton::Left),
                .shootDirection = {static_cast<float>(m_graphicModule->getMousePosition().x), static_cast<float>(m_graphicModule->getMousePosition().y)}
            });

        m_ecs->getComponent<Rte::BasicComponents::Transform>(sky).position.x -= 5;
        if (m_ecs->getComponent<Rte::BasicComponents::Transform>(sky).position.x <= -1920)
            m_ecs->getComponent<Rte::BasicComponents::Transform>(sky).position.x = 1920;
        m_ecs->getComponent<Rte::BasicComponents::Transform>(sky1).position.x -= 5;
        if (m_ecs->getComponent<Rte::BasicComponents::Transform>(sky1).position.x <= -1920)
            m_ecs->getComponent<Rte::BasicComponents::Transform>(sky1).position.x = 1920;

        m_entitiesMutex.lock(); {
            m_graphicModule->update();
            m_networkModuleClient->sendUpdate();
        } m_entitiesMutex.unlock();
    }

    networkThread.join();
}

void ClientApp::handleConsoleInput() {
    m_devConsoleHistory.emplace_back(">> " + m_devConsoleInput);
    if (m_devConsoleInput == "exit") {
        m_running = false;
    }
    else if (m_devConsoleInput == "clear") {
        m_devConsoleHistory.clear();
    }
    else {
        m_devConsoleHistory.emplace_back("Unknown command :( ");
    }
}
