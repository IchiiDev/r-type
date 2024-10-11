#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>
#include <iostream>


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

static float getDistanceFrome2Points(const Rte::Vec2<float>& p1, const Rte::Vec2<float>& p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
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

ServerApp::ServerApp() {
    m_ecs = std::make_shared<Rte::Ecs>();

    // Load the graphic module
    m_graphicModule = Rte::interfaceCast<Rte::Graphic::GraphicModule>(moduleManager.loadModule("RteGraphic"));
    m_graphicModule->init(m_ecs);
    m_graphicModule->setWindowTitle("R-Type");
    m_graphicModule->setWindowSize({1920, 1080});
    m_graphicModule->setDaltonismMode(Rte::Graphic::DaltonismMode::NONE);
    m_graphicModule->loadFontFromFile("../assets/alagard.ttf");
    m_graphicModule->setLayerCount(10);

    // Load the physics module
    m_physicsModule = Rte::interfaceCast<Rte::Physics::PhysicsModule>(moduleManager.loadModule("RtePhysics"));
    m_physicsModule->init(m_ecs);

    // Load the network module
    m_networkModule = Rte::interfaceCast<Rte::Network::NetworkModule>(moduleManager.loadModule("RteNetwork"));
    m_networkModuleServer = m_networkModule->getServer();
}

void ServerApp::run() {

    m_networkModuleServer->init(m_ecs);
    m_networkModuleServer->start(123456);

    // Creation of a a background entity
    std::vector<Rte::Entity> breakableEntities;
    const Rte::Vec2<Rte::u16> windowSize = m_graphicModule->getWindowSize();

    // Creation of a copper bar entity
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

    Player player(m_ecs, m_graphicModule, m_physicsModule);

    // Creation of the enemies vector

    std::vector<Enemy> enemies;

    // Creation of a enemy entity

    enemies.emplace_back(m_ecs, m_graphicModule, m_physicsModule);

    // Invisible walls

    Rte::Entity leftwall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(leftwall, Rte::BasicComponents::Transform{
        .position = {-100, 0},
        .scale = {8, 8},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(leftwall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {100, 10000},
        1,
        0.3,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(leftwall).position,
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    Rte::Entity rightWall = m_ecs->createEntity();
    m_ecs->addComponent<Rte::BasicComponents::Transform>(rightWall, Rte::BasicComponents::Transform{
        .position = {2020, 0},
        .scale = {8, 8},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(rightWall, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {100, 10000},
        1,
        0.3,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(rightWall).position,
        0,
        true,
        true,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    std::shared_ptr<std::vector<Rte::Entity>> allEntities = std::make_shared<std::vector<Rte::Entity>>();
    allEntities->push_back(sandBoxEntity);
    allEntities->push_back(leftwall);
    allEntities->push_back(rightWall);
    allEntities->push_back(player.getEntity());
    for (auto & enemie : enemies) {
        allEntities->push_back(enemie.getEntity());
    }
    for (auto & breakableEntity : breakableEntities) {
        allEntities->push_back(breakableEntity);
    }

    while (true) {
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Left) || m_graphicModule->isKeyPressed(Rte::Graphic::Key::Q) ) {
            player.move({-10, 0});
        }
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Right) || m_graphicModule->isKeyPressed(Rte::Graphic::Key::D)) {
            player.move({10, 0});
        }
        if (m_graphicModule->isKeyPressed(Rte::Graphic::Key::Space)) {
            player.fly({0, 1});
        }

        // Update the canvas (sandbox pixels)
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
        
        // Update the entities
        m_physicsModule->update();
        player.update();
        enemies.at(0).update();
        if (rand() % 10 == 0) {
            enemies.at(0).shoot(player.getPos());
        }
        while (true) {
            Rte::Vec2<float> pos = player.getDestroyedProjectilePos();
            if (pos.x == 0 && pos.y == 0)
                break;
            breakEntities(m_graphicModule, m_physicsModule, breakableEntities, {static_cast<unsigned short>(pos.x + windowSize.x / 2), static_cast<unsigned short>(pos.y + windowSize.y / 2)}, m_ecs);
        }
        for (auto & enemie : enemies) {
            while (true) {
                Rte::Vec2<float> pos = enemie.getDestroyedProjectilePos();
                if (pos.x == 0 && pos.y == 0)
                    break;
                breakEntities(m_graphicModule, m_physicsModule, breakableEntities, {static_cast<unsigned short>(pos.x + windowSize.x / 2), static_cast<unsigned short>(pos.y + windowSize.y / 2)}, m_ecs);
                if (getDistanceFrome2Points(pos, player.getPos()) < 100) {
                    player.takeDamage();
                }
            }
        }

        // Re update the entities
        allEntities->clear();
        allEntities->push_back(sandBoxEntity);
        allEntities->push_back(leftwall);
        allEntities->push_back(rightWall);
        allEntities->push_back(player.getEntity());
        for (auto & enemie : enemies) {
            allEntities->push_back(enemie.getEntity());
        }
        for (auto & breakableEntity : breakableEntities) {
            allEntities->push_back(breakableEntity);
        }

        m_graphicModule->update();

        m_networkModuleServer->updateEntity(allEntities);
        m_networkModuleServer->update();
    }
}
