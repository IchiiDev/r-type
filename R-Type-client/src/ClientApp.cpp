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
#include <algorithm>
#include <stack>
ClientApp::ClientApp() {
    m_ecs = std::make_shared<Rte::Ecs>();
}

//Where image is an array of pixels in rgba format
std::vector<int> convertToBinary(const Rte::u8* image, Rte::Vec2<Rte::u16> size) {
    std::vector<int> binaryImage;
    for (int i = 1; i < size.x * size.y + 1; i++)
    {
        if (image[i * 4 - 1] == 0)
        {
            binaryImage.push_back(0);
            std::cout << " ";
        }
        else
        {
            binaryImage.push_back(1);
            std::cout << "X";
        }
        if (i % size.x == 0)
        {
            std::cout << std::endl;
        }
    }
    return binaryImage;
}

std::vector<std::vector<int>> connectedComponentLabeling(const std::vector<int>& binaryPixels, Rte::Vec2<uint16_t> size) {
    int width = size.x;
    int height = size.y;
    std::vector<std::vector<int>> labels(height, std::vector<int>(width, 0));
    int currentLabel = 1;

    auto getIndex = [width](int x, int y) {
        return y * width + x;
    };

    auto isValid = [width, height](int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (binaryPixels[getIndex(x, y)] == 1 && labels[y][x] == 0) {
                std::stack<std::pair<int, int>> stack;
                stack.push({x, y});
                labels[y][x] = currentLabel;

                while (!stack.empty()) {
                    auto [cx, cy] = stack.top();
                    stack.pop();

                    for (const auto& [dx, dy] : directions) {
                        int nx = cx + dx;
                        int ny = cy + dy;

                        if (isValid(nx, ny) && binaryPixels[getIndex(nx, ny)] == 1 && labels[ny][nx] == 0) {
                            stack.push({nx, ny});
                            labels[ny][nx] = currentLabel;
                        }
                    }
                }

                ++currentLabel;
            }
        }
    }

    std::vector<std::vector<int>> labeledImages(currentLabel - 1, std::vector<int>(binaryPixels.size(), 0));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int label = labels[y][x];
            if (label > 0) {
                labeledImages[label - 1][getIndex(x, y)] = 1;
            }
        }
    }

    return labeledImages;
}

Rte::u8* andImage(Rte::u8* image, std::vector<int> binaryImage, Rte::Vec2<Rte::u16> size) {
    Rte::u8* newImage = new Rte::u8[size.x * size.y * 4];
    for (int i = 0; i < size.x * size.y; i++)
    {
        if (binaryImage[i] == 1)
        {
            newImage[i * 4] = image[i * 4];
            newImage[i * 4 + 1] = image[i * 4 + 1];
            newImage[i * 4 + 2] = image[i * 4 + 2];
            newImage[i * 4 + 3] = image[i * 4 + 3];
        }
        else
        {
            newImage[i * 4] = 0;
            newImage[i * 4 + 1] = 0;
            newImage[i * 4 + 2] = 0;
            newImage[i * 4 + 3] = 0;
        }
    }
    return newImage;
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
    texture->loadFromFile("../sprite4.png");
    
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

    // Callback to print mouse button pressed
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::MOUSE_BUTTON_PRESSED, [&](Rte::Event& event) {
        const Rte::Graphic::MouseButton button = event.getParameter<Rte::Graphic::MouseButton>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED);
        const Rte::Vec2<Rte::u16> position = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED_POSITION);
        std::cout << "Mouse button pressed: " << static_cast<int>(button) << " at position (" << position.x << ", " << position.y << ")\n";
        
        const std::shared_ptr<Rte::Graphic::Texture> newTexture = graphicModule->createTexture();
        bool hasChanged = false;
        Rte::u8 *newPixels = physicsModule->fractureRigidBody(rigidBody, position, hasChanged);
        Rte::Vec2<Rte::u16> size = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(entity).texture->getSize();
        if (hasChanged) {
            std::cout << "Fracture" << std::endl;
            std::vector<int> binaryPixels = convertToBinary(newPixels, size);
            std::vector<std::vector<int>> components = connectedComponentLabeling(binaryPixels, size);
            for (int i = 0; i < components.size(); i++) {
                const std::shared_ptr<Rte::Graphic::Texture> newTexture = graphicModule->createTexture();
                Rte::u8 *componentsPixels =  andImage(newPixels, components[i], size);
                newTexture->loadFromMemory(componentsPixels, size);
                Rte::Entity newEntity = m_ecs->createEntity();
                m_ecs->addComponent<Rte::Graphic::Components::Sprite>(newEntity, Rte::Graphic::Components::Sprite(newTexture));
                std::shared_ptr<Rte::Physics::RigidBody> newRigidBody = physicsModule->createRigidBody(rigidBody, componentsPixels, size);
                m_ecs->addComponent<Rte::Physics::Components::Physics>(newEntity, Rte::Physics::Components::Physics{newRigidBody});
                m_ecs->addComponent<Rte::BasicComponents::Transform>(newEntity, Rte::BasicComponents::Transform{
                    .position = entityPosition,
                    .scale = entityScale,
                    .rotation = 0
                });
            }
            physicsModule->destroyRigidBody(rigidBody);
            m_ecs->destroyEntity(entity);
        }
    }));

    // Main loop
    while (running) {
        physicsModule->update();
        graphicModule->update();
    }
}
