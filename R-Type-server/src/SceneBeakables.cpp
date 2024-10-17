#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>
#include <stack>

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

void ServerApp::fractureBreakable(const Rte::Vec2<Rte::u16>& position) {
    for (int i = 0; i < m_breakables.size(); i++) {
        bool hasChanged = false;
        std::vector<Rte::u8> newMatPixels = m_physicsModule->fractureRigidBody(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_breakables.at(i)).rigidBody, position, hasChanged);
        if (hasChanged) {
            const Rte::Vec2<Rte::u16> size = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(*m_breakables.at(i)).texture->getSize();
            const std::vector<int> binaryImage = convertToBinary(newMatPixels.data(), size);
            const std::vector<std::vector<int>> components = connectedComponentLabeling(binaryImage, size);
            for (const auto & component : components) {
                const std::shared_ptr<Rte::Graphic::Texture> newTexture = m_graphicModule->createTexture();
                const std::shared_ptr<Rte::Graphic::Texture> newMaterial = m_graphicModule->createTexture();
                const std::vector<Rte::u8> componentTexPixels = andImage(m_ecs->getComponent<Rte::Graphic::Components::Sprite>(*m_breakables.at(i)).texture->getPixels(), component, size);
                const std::vector<Rte::u8> componentMatPixels = andImage(newMatPixels.data(), component, size);
                newTexture->loadFromMemory(componentTexPixels.data(), size);
                newMaterial->loadFromMemory(componentMatPixels.data(), size);
                createBreakable(*m_breakables.at(i), componentTexPixels, componentMatPixels, size);
            }
            destroyBreakable(*m_breakables.at(i));
        }
    }
}

void ServerApp::createBreakable(Rte::Vec2<float> pos, std::string spritePath) {
    // Add breakable to the entities list
    Rte::Entity breakable = m_ecs->createEntity();
    // Load texture
    const std::shared_ptr<Rte::Graphic::Texture> breakableTexture = m_graphicModule->createTexture();
    breakableTexture->loadFromFile("../assets/" + spritePath + "-tex.png");
    const std::shared_ptr<Rte::Graphic::Texture> breakableMaterial = m_graphicModule->createTexture();
    breakableMaterial->loadFromFile("../assets/" + spritePath + "-mat.png");
    
    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(breakable, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakable, Rte::Graphic::Components::Sprite{breakableTexture});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakable, Rte::BasicComponents::Transform{
        .position = {pos.x + static_cast<float>(m_graphicModule->getWindowSize().x) / 2, pos.y + static_cast<float>(m_graphicModule->getWindowSize().y) / 2},
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakable, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        breakableMaterial->getPixels(),
        breakableMaterial->getSize(),
        pos,
        0
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(breakable));

    // Add breakable to the entities list
    m_entities->emplace_back(breakable);

    // Load texture and add to new entities textures
    auto texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(breakable).texture;
    std::vector<Rte::u8> pixelsVector(texture->getPixels(), texture->getPixels() + static_cast<ptrdiff_t>(texture->getSize().x * texture->getSize().y) * 4);
    
    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = texture->getSize();
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[breakable] = packedTexture;
}

void ServerApp::createBreakable(Rte::Vec2<float> pos, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size) {
    // Add breakable to the entities list
    Rte::Entity breakable = m_ecs->createEntity();
    // Load texture
    const std::shared_ptr<Rte::Graphic::Texture> breakableTexture = m_graphicModule->createTexture();
    breakableTexture->loadFromMemory(texture.data(), size);
    const std::shared_ptr<Rte::Graphic::Texture> breakableMaterial = m_graphicModule->createTexture();
    breakableMaterial->loadFromMemory(material.data(), size);
    
    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(breakable, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakable, Rte::Graphic::Components::Sprite{breakableTexture});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakable, Rte::BasicComponents::Transform{
        .position = {pos.x + static_cast<float>(m_graphicModule->getWindowSize().x) / 2, pos.y + static_cast<float>(m_graphicModule->getWindowSize().y) / 2},
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakable, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        breakableMaterial->getPixels(),
        breakableMaterial->getSize(),
        pos,
        0
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(breakable));

    // Add breakable to the entities list
    m_entities->emplace_back(breakable);

    // Load texture and add to new entities textures
    std::vector<Rte::u8> pixelsVector(breakableTexture->getPixels(), breakableTexture->getPixels() + static_cast<ptrdiff_t>(breakableTexture->getSize().x * breakableTexture->getSize().y) * 4);
    
    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = breakableTexture->getSize();
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[breakable] = packedTexture;
}

void ServerApp::createBreakable(Rte::Entity breakable, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size) {
    // Add breakable to the entities list
    Rte::Entity newBreakables = m_ecs->createEntity();
    // Load texture
    const std::shared_ptr<Rte::Graphic::Texture> breakableTexture = m_graphicModule->createTexture();
    breakableTexture->loadFromMemory(texture.data(), size);
    const std::shared_ptr<Rte::Graphic::Texture> breakableMaterial = m_graphicModule->createTexture();
    breakableMaterial->loadFromMemory(material.data(), size);
    
    Rte::Vec2<float> pos = m_ecs->getComponent<Rte::BasicComponents::Transform>(breakable).position;

    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(newBreakables, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(newBreakables, Rte::Graphic::Components::Sprite{breakableTexture});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(newBreakables, Rte::BasicComponents::Transform{
        .position = pos,
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(newBreakables, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        m_ecs->getComponent<Rte::Physics::Components::Physics>(breakable).rigidBody,
        breakableMaterial->getPixels(),
        breakableMaterial->getSize()
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(newBreakables));

    // Add breakable to the entities list
    m_entities->emplace_back(newBreakables);

    // Load texture and add to new entities textures
    std::vector<Rte::u8> pixelsVector(breakableTexture->getPixels(), breakableTexture->getPixels() + static_cast<ptrdiff_t>(breakableTexture->getSize().x * breakableTexture->getSize().y) * 4);
    
    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = breakableTexture->getSize();
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[newBreakables] = packedTexture;
}

void ServerApp::updateBreakables() {
    for (size_t i = 0; i < m_breakables.size(); i++) {
        // projectile collision
        for (size_t j = 0; j < m_projectiles.size(); j++) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[j]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_breakables[i]).rigidBody)) {
                fractureBreakable({static_cast<unsigned short>(m_ecs->getComponent<Rte::BasicComponents::Transform>(*m_projectiles[j]).position.x + 1920 / 2), static_cast<unsigned short>(m_ecs->getComponent<Rte::BasicComponents::Transform>(*m_projectiles[j]).position.y + 1080 / 2)});
                destroyProjectile(*m_projectiles[j]);
            }
        }
    }
}

void ServerApp::destroyBreakable(const Rte::Entity& breakable) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(breakable);
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(std::next(m_entities->begin(), static_cast<std::ptrdiff_t>(j)));
            break;
        }
    }
    m_ecs->destroyEntity(breakable);
    for (size_t i = 0; i < m_breakables.size(); i++)
        if (*m_breakables[i] == breakable)
            m_breakables.erase(std::next(m_breakables.begin(), static_cast<std::ptrdiff_t>(i)));
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}
