#include "ClientApp.hpp"

#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stack>
#include <utility>
#include <vector>

Rte::Physics::Color randomColor(Rte::Physics::Color color, int offset) {
    const int random = static_cast<int>(static_cast<float>(rand() % 3) / 3.F * static_cast<float>(offset));
    color.r += random - offset / 2;
    color.g += random - offset / 2;
    color.b += random - offset / 2;

    color.r = std::clamp(color.r, 0, 255);
    color.g = std::clamp(color.g, 0, 255);
    color.b = std::clamp(color.b, 0, 255);

    return color;
}

std::vector<int> convertToBinary(const Rte::u8* image, const Rte::Vec2<Rte::u16>& size) {
    std::vector<int> binaryImage;
    for (int i = 1; i < size.x * size.y + 1; i++) {
        if (image[i * 4 - 1] == 0)
            binaryImage.push_back(0);
        else
            binaryImage.push_back(1);
    }

    return binaryImage;
}

std::vector<std::vector<int>> connectedComponentLabeling(const std::vector<int>& binaryPixels, const Rte::Vec2<Rte::u16>& size) {   // NOLINT(readability-function-cognitive-complexity)
    const int width = size.x;
    const int height = size.y;

    std::vector<std::vector<int>> labels(height, std::vector<int>(width, 0));
    int currentLabel = 1;

    auto getIndex = [width](int x, int y) {
        return y * width + x;
    };

    auto isValid = [width, height](int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    const std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (binaryPixels.at(getIndex(x, y)) == 1 && labels.at(y).at(x) == 0) {
                std::stack<std::pair<int, int>> stack;
                stack.emplace(x, y);
                labels.at(y).at(x) = currentLabel;

                while (!stack.empty()) {
                    auto [cx, cy] = stack.top();
                    stack.pop();

                    for (const auto& [dx, dy] : directions) {
                        const int nx = cx + dx;
                        const int ny = cy + dy;

                        if (isValid(nx, ny) && binaryPixels.at(getIndex(nx, ny)) == 1 && labels.at(ny).at(nx) == 0) {
                            stack.emplace(nx, ny);
                            labels.at(ny).at(nx) = currentLabel;
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
            const int label = labels.at(y).at(x);
            if (label > 0)
                labeledImages.at(label - 1).at(getIndex(x, y)) = 1;
        }
    }

    return labeledImages;
}

std::vector<Rte::u8> andImage(const Rte::u8* image, const std::vector<int>& binaryImage, Rte::Vec2<Rte::u16> size) {
    std::vector<Rte::u8> newImage(static_cast<size_t>(size.x * size.y * 4), 0);
    for (int i = 0; i < size.x * size.y; i++) {
        if (binaryImage.at(i) == 1) {
            newImage.at(static_cast<long long>(static_cast<long>(i)) * 4) = image[static_cast<ptrdiff_t>(i * 4)];
            newImage.at(i * 4 + 1) = image[i * 4 + 1];
            newImage.at(i * 4 + 2) = image[i * 4 + 2];
            newImage.at(i * 4 + 3) = image[i * 4 + 3];
        }
    }

    return newImage;
}

float getRotFromPoints(const Rte::Vec2<float> p1, const Rte::Vec2<float> p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

void breakEntities(const std::shared_ptr<Rte::Graphic::GraphicModule>& m_graphicModule, const std::shared_ptr<Rte::Physics::PhysicsModule>& m_physicsModule, std::vector<Rte::Entity>& breakableEntities, const Rte::Vec2<Rte::u16>& position, const std::shared_ptr<Rte::Ecs>& ecs) {
    for (int i = 0; i < breakableEntities.size(); i++) {
        bool hasChanged = false;
        std::vector<Rte::u8> newMatPixels = m_physicsModule->fractureRigidBody(ecs->getComponent<Rte::Physics::Components::Physics>(breakableEntities.at(i)).rigidBody, position, hasChanged);
        if (hasChanged) {
            const Rte::Vec2<Rte::u16> size = ecs->getComponent<Rte::Graphic::Components::Sprite>(breakableEntities.at(i)).texture->getSize();
            const std::vector<int> binaryImage = convertToBinary(newMatPixels.data(), size);
            const std::vector<std::vector<int>> components = connectedComponentLabeling(binaryImage, size);
            for (const auto & component : components) {
                const std::shared_ptr<Rte::Graphic::Texture> newTexture = m_graphicModule->createTexture();
                const std::shared_ptr<Rte::Graphic::Texture> newMaterial = m_graphicModule->createTexture();
                const std::vector<Rte::u8> componentTexPixels = andImage(ecs->getComponent<Rte::Graphic::Components::Sprite>(breakableEntities.at(i)).texture->getPixels(), component, size);
                const std::vector<Rte::u8> componentMatPixels = andImage(newMatPixels.data(), component, size);
                newTexture->loadFromMemory(componentTexPixels.data(), size);
                newMaterial->loadFromMemory(componentMatPixels.data(), size);

                breakableEntities.push_back(ecs->createEntity());
                ecs->addComponent<Rte::Graphic::Components::Sprite>(breakableEntities.at(breakableEntities.size() - 1), Rte::Graphic::Components::Sprite(newTexture));
                const std::shared_ptr<Rte::Physics::RigidBody> newRigidBody = m_physicsModule->createRigidBody(ecs->getComponent<Rte::Physics::Components::Physics>(breakableEntities.at(i)).rigidBody, componentMatPixels.data(), size);
                ecs->addComponent<Rte::Physics::Components::Physics>(breakableEntities.at(breakableEntities.size() - 1), Rte::Physics::Components::Physics{newRigidBody});
                ecs->addComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1), Rte::BasicComponents::Transform{ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(i))});
            }
            ecs->destroyEntity(breakableEntities.at(i));
            breakableEntities.erase(std::remove(breakableEntities.begin(), breakableEntities.end(), breakableEntities.at(i)), breakableEntities.end());
        }
    }
}

void ClientApp::gameplayLoop() {
    // Creation of a a background entity
    std::vector<Rte::Entity> breakableEntities;

    const std::shared_ptr<Rte::Graphic::Texture> backgroundTexture = m_graphicModule->createTexture();
    backgroundTexture->loadFromFile("../assets/background.png");
    constexpr Rte::Vec2<float> backgroundScale = {8, 8};
    const Rte::Vec2<Rte::u16> windowSize = m_graphicModule->getWindowSize();
    const Rte::Vec2<float> backgroundPosition = {
        0,
        0
    };
    const Rte::Entity backgroundEntity = m_ecs->createEntity();
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(backgroundEntity, Rte::Graphic::Components::Sprite(backgroundTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(backgroundEntity, Rte::BasicComponents::Transform{
        .position = backgroundPosition,
        .scale = backgroundScale,
        .rotation = 0,
    });

    // Creation of a texture
    const std::shared_ptr<Rte::Graphic::Texture> texture = m_graphicModule->createTexture();
    texture->loadFromFile("../assets/copper-bar-tex.png");
    const std::shared_ptr<Rte::Graphic::Texture> material = m_graphicModule->createTexture();
    material->loadFromFile("../assets/copper-bar-mat.png");

    constexpr Rte::Vec2<float> entityScale = {8, 8};
    const Rte::Vec2<float> entityPosition = {
        0,
        135.F / 2 * 8 - (static_cast<float>(texture->getSize().y) / 2 * 8) + 16
    };
    breakableEntities.push_back(m_ecs->createEntity());

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakableEntities.at(breakableEntities.size() - 1), Rte::Graphic::Components::Sprite(texture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1), Rte::BasicComponents::Transform{
        .position = entityPosition,
        .scale = entityScale,
        .rotation = 0,
    });


    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakableEntities.at(breakableEntities.size() - 1), Rte::Physics::Components::Physics{m_physicsModule->createRigidBody(
        material->getPixels(),
        material->getSize(),
        m_ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1)).position,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1)).rotation
    )});


    // Creation of second entity
    const std::shared_ptr<Rte::Graphic::Texture> textureM = m_graphicModule->createTexture();
    textureM->loadFromFile("../assets/mushroom-tex.png");
    const std::shared_ptr<Rte::Graphic::Texture> materialM = m_graphicModule->createTexture();
    materialM->loadFromFile("../assets/mushroom-mat.png");

    constexpr Rte::Vec2<float> entityScaleM = {8, 8};
    const Rte::Vec2<float> entityPositionM = {
        0,
        135.F / 2 * 8 - (static_cast<float>(texture->getSize().y) / 2 * 8) - 256 - 64 + 16
    };
    breakableEntities.push_back(m_ecs->createEntity());

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakableEntities.at(breakableEntities.size() - 1), Rte::Graphic::Components::Sprite(textureM));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1), Rte::BasicComponents::Transform{
        .position = entityPositionM,
        .scale = entityScaleM,
        .rotation = 0,
    });


    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakableEntities.at(breakableEntities.size() - 1), Rte::Physics::Components::Physics{m_physicsModule->createRigidBody(
        materialM->getPixels(),
        materialM->getSize(),
        m_ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1)).position,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(breakableEntities.size() - 1)).rotation
    )});

    // Creation of the sandBox

    constexpr Rte::Vec2<float> sandBoxScale = {8, 8};
    const Rte::Vec2<Rte::u16> sandBoxSize = {240, 135};
    const Rte::Vec2<float> sandBoxPosition = {
        0,
        0
    };

    const std::shared_ptr<Rte::Graphic::Texture> sandBoxTexture = m_graphicModule->createTexture();
    std::vector<Rte::u8> tempSandBox(static_cast<size_t>(sandBoxSize.x * sandBoxSize.y * 4), 0);
    sandBoxTexture->loadFromMemory(tempSandBox.data(), sandBoxSize);

    const Rte::Entity sandBoxEntity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(sandBoxEntity, Rte::Graphic::Components::Sprite(sandBoxTexture));

    m_ecs->addComponent<Rte::BasicComponents::Transform>(sandBoxEntity, Rte::BasicComponents::Transform{
        .position = sandBoxPosition,
        .scale = sandBoxScale,
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(sandBoxEntity, Rte::Physics::Components::Physics{.sandBox = m_physicsModule->createSandBox(sandBoxSize)});

    Rte::Physics::MaterialType k = Rte::Physics::MaterialType::SAND;

    // Creation of a player entity

    const std::shared_ptr<Rte::Graphic::Texture> playerTexture = m_graphicModule->createTexture();
    playerTexture->loadFromFile("../assets/player.png");

    constexpr Rte::Vec2<float> playerScale = {3, 3};
    const Rte::Vec2<float> playerPosition = {
        0,
        0
    };

    const Rte::Entity playerEntity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(playerEntity, Rte::Graphic::Components::Sprite(playerTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(playerEntity, Rte::BasicComponents::Transform{
        .position = playerPosition,
        .scale = playerScale,
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(playerEntity, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {31 * 3, 47 * 3},
        1,
        50,
        playerPosition,
        0,
        true
    )});

    // Creation of a crosshair entity

    const std::shared_ptr<Rte::Graphic::Texture> crosshairTexture = m_graphicModule->createTexture();
    crosshairTexture->loadFromFile("../assets/crosshair.png");

    constexpr Rte::Vec2<float> crosshairScale = {8, 8};
    const Rte::Vec2<float> crosshairPosition = {
        1000,
        1000
    };

    const Rte::Entity crosshairEntity = m_ecs->createEntity();

    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(crosshairEntity, Rte::Graphic::Components::Sprite(crosshairTexture));
    m_ecs->addComponent<Rte::BasicComponents::Transform>(crosshairEntity, Rte::BasicComponents::Transform{
        .position = crosshairPosition,
        .scale = crosshairScale,
        .rotation = 0
    });

    // Callback to move the sprite and make it at the center of the window
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::RESIZED,
        [&](Rte::Event& event) {
            // Get parameters from the event
            const Rte::Vec2<Rte::u16> newSize = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::NEW_WINDOW_SIZE);

            // Update the sprite position
            Rte::BasicComponents::Transform& transform = m_ecs->getComponent<Rte::BasicComponents::Transform>(breakableEntities.at(0));
            transform.position.x = (static_cast<float>(newSize.x) / 2) - (entityScale.x / 2);
            transform.position.y = (static_cast<float>(newSize.y) / 2) - (entityScale.y / 2);
        }
    ));

    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::KEY_PRESSED, [&](Rte::Event& event) {
        const Rte::Graphic::Key key = event.getParameter<Rte::Graphic::Key>(Rte::Graphic::Events::Params::KEY_PRESSED);
        std::cout << "Key pressed: " << static_cast<int>(key) << "\n";
    }));

    // Callback to print mouse button pressed
    m_ecs->addEventListener(LAMBDA_LISTENER(Rte::Graphic::Events::MOUSE_BUTTON_PRESSED, [&](Rte::Event& event) {
        const Rte::Graphic::MouseButton button = event.getParameter<Rte::Graphic::MouseButton>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED);
        const Rte::Vec2<Rte::u16> position = event.getParameter<Rte::Vec2<Rte::u16>>(Rte::Graphic::Events::Params::MOUSE_BUTTON_PRESSED_POSITION);
        std::cout << "Mouse button pressed: " << static_cast<int>(button) << " at position (" << position.x << ", " << position.y << ")\n";
        breakEntities(m_graphicModule, m_physicsModule, breakableEntities, position, m_ecs);
        Rte::Vec2<float> playerPos = m_ecs->getComponent<Rte::BasicComponents::Transform>(playerEntity).position;
        float angle = getRotFromPoints(playerPos, {static_cast<float>(position.x - windowSize.x / 2), static_cast<float>(position.y - windowSize.y / 2)});
        const std::shared_ptr<Rte::Graphic::Texture> projectileTexture = m_graphicModule->createTexture();
        projectileTexture->loadFromFile("../assets/projectile.png");
        Rte::Entity projectileEntity = m_ecs->createEntity();
        m_ecs->addComponent<Rte::Graphic::Components::Sprite>(projectileEntity, Rte::Graphic::Components::Sprite(projectileTexture));
        m_ecs->addComponent<Rte::BasicComponents::Transform>(projectileEntity, Rte::BasicComponents::Transform{
            .position = {static_cast<float>(cos(angle) * 100) + playerPos.x, static_cast<float>(sin(angle) * 100) + playerPos.y},
            .scale = {8, 8},
            .rotation = -angle
        });
        m_ecs->addComponent<Rte::Physics::Components::Physics>(projectileEntity, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
            {64, 0},
            1,
            0.3,
            {m_ecs->getComponent<Rte::BasicComponents::Transform>(projectileEntity).position.x + m_graphicModule->getWindowSize().x / 2,
             m_ecs->getComponent<Rte::BasicComponents::Transform>(projectileEntity).position.y + m_graphicModule->getWindowSize().y / 2},
            m_ecs->getComponent<Rte::BasicComponents::Transform>(projectileEntity).rotation,
            false
        )});

        m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(projectileEntity).shapeBody, {
            static_cast<float>(cos(angle) * 10),
            static_cast<float>(sin(-angle) * 10)
        });
    }));

    // Main loop
    while (m_running) {
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::S))
            k = Rte::Physics::MaterialType::SAND;
        else if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::W))
            k = Rte::Physics::MaterialType::WATER;
        else if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::B))
            k = Rte::Physics::MaterialType::STATIC_WOOD;
        else if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::A))
            k = Rte::Physics::MaterialType::ACID;
        
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Left)) {
            m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(playerEntity).shapeBody, {-10, 0});
        }
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Right)) {
            m_physicsModule->move(m_ecs->getComponent<Rte::Physics::Components::Physics>(playerEntity).shapeBody, {10, 0});
        }
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Space)) {
            m_physicsModule->applyForce(m_ecs->getComponent<Rte::Physics::Components::Physics>(playerEntity).shapeBody, {0, 1});
        }
        
        m_ecs->getComponent<Rte::BasicComponents::Transform>(crosshairEntity).position = {
            static_cast<float>(m_graphicModule->getMousePosition().x - m_graphicModule->getWindowSize().x / 2),
            static_cast<float>(m_graphicModule->getMousePosition().y - m_graphicModule->getWindowSize().y / 2)
        };

        if (m_graphicModule->isMouseButtonPressed(Rte::Graphic::MouseButton::Left)) {
            const Rte::Vec2<Rte::u16> position = m_graphicModule->getMousePosition();
            m_physicsModule->changeSandBoxPixel(sandBoxEntity, {position.x / 8, position.y / 8},     {k, randomColor(Rte::Physics::invMatColors.at(k), 60), 0});
            m_physicsModule->changeSandBoxPixel(sandBoxEntity, {position.x / 8 + 1, position.y / 8}, {k, randomColor(Rte::Physics::invMatColors.at(k), 60), 0});
            m_physicsModule->changeSandBoxPixel(sandBoxEntity, {position.x / 8 - 1, position.y / 8}, {k, randomColor(Rte::Physics::invMatColors.at(k), 60), 0});
            m_physicsModule->changeSandBoxPixel(sandBoxEntity, {position.x / 8, position.y / 8 + 1}, {k, randomColor(Rte::Physics::invMatColors.at(k), 60), 0});
            m_physicsModule->changeSandBoxPixel(sandBoxEntity, {position.x / 8, position.y / 8 - 1}, {k, randomColor(Rte::Physics::invMatColors.at(k), 60), 0});
        }

        std::vector<Rte::Physics::Pixel> canvas = m_physicsModule->getSandBoxCanvas(m_ecs->getComponent<Rte::Physics::Components::Physics>(sandBoxEntity).sandBox);
        for (int i = 0; i < sandBoxSize.x * sandBoxSize.y; i++) {
            tempSandBox.at(static_cast<long long>(static_cast<long>(i)) * 4) = canvas.at(i).color.r;
            tempSandBox.at(i * 4 + 1) = canvas.at(i).color.g;
            tempSandBox.at(i * 4 + 2) = canvas.at(i).color.b;
            tempSandBox.at(i * 4 + 3) = canvas.at(i).color.a;
        }

        sandBoxTexture->loadFromMemory(tempSandBox.data(), sandBoxSize);
        m_ecs->removeComponent<Rte::Graphic::Components::Sprite>(sandBoxEntity);
        m_ecs->addComponent<Rte::Graphic::Components::Sprite>(sandBoxEntity, Rte::Graphic::Components::Sprite(sandBoxTexture));
        m_physicsModule->update();
        m_graphicModule->update();
    }
}
