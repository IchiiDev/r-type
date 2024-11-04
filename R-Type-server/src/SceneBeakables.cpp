#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/Tool.hpp"
#include "ServerApp.hpp"
#include "Utils/BinaryMap.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stack>

void ServerApp::fractureBreakable(const Rte::Vec2<Rte::u16>& position) {
    for (int i = 0; i < m_breakables.size(); i++) {
        bool hasChanged = false;
        std::vector<Rte::u8> newMatPixels = m_physicsModule->fractureRigidBody(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_breakables.at(i)).rigidBody, position, m_destructionMaps["round1"].second, m_destructionMaps["round1"].first, hasChanged);
        if (hasChanged) {
            const Rte::Vec2<Rte::u16> size = m_graphicModule->getTextureSize(m_ecs->getComponent<Rte::Graphic::Components::Sprite>(*m_breakables.at(i)).textureId);
            const std::vector<int> binaryImage = convertToBinary(newMatPixels.data(), size);
            const std::vector<std::vector<int>> components = connectedComponentLabeling(binaryImage, size);
            for (const auto & component : components) {
                uint32_t newTexture = m_graphicModule->createTexture();
                uint32_t newMaterial = m_graphicModule->createTexture();

                const std::vector<Rte::u8> componentTexPixels = andImage(m_graphicModule->getTexturePixels(m_ecs->getComponent<Rte::Graphic::Components::Sprite>(*m_breakables.at(i)).textureId), component, size);
                const std::vector<Rte::u8> componentMatPixels = andImage(newMatPixels.data(), component, size);

                if (!m_graphicModule->loadTextureFromMemory(newTexture, componentTexPixels.data(), size))
                    throw std::runtime_error("Failed to load texture from memory");
                if (!m_graphicModule->loadTextureFromMemory(newMaterial, componentMatPixels.data(), size))
                    throw std::runtime_error("Failed to load texture from memory");

                createBreakable(*m_breakables.at(i), componentTexPixels, componentMatPixels, size);
            }
            destroyBreakable(*m_breakables.at(i));
        }
    }
}

Rte::Entity ServerApp::createBreakable(Rte::Vec2<float> pos, std::string spritePath) {
    // Add breakable to the entities list
    Rte::Entity breakable = m_ecs->createEntity();
    // Load texture
    uint32_t breakableTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(breakableTexture, std::string("../assets/" + spritePath + "-tex.png").c_str()))
        throw std::runtime_error("Failed to load texture: \"../assets/" + spritePath + "-tex.png\"");

    uint32_t breakableMaterial = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromFile(breakableMaterial, std::string("../assets/" + spritePath + "-mat.png").c_str()))
        throw std::runtime_error("Failed to load texture: \"../assets/" + spritePath + "-mat.png\"");

    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(breakable, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakable, Rte::Graphic::Components::Sprite{.textureId = breakableTexture, .offset = {0, 0}, .layer = 0});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakable, Rte::BasicComponents::Transform{
        .position = {pos.x + static_cast<float>(m_graphicModule->getWindowSize().x) / 2, pos.y + static_cast<float>(m_graphicModule->getWindowSize().y) / 2},
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakable, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        m_graphicModule->getTexturePixels(breakableMaterial),
        m_graphicModule->getTextureSize(breakableMaterial),
        pos,
        0
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(breakable));

    // Add breakable to the entities list
    m_entitiesMutex.lock();
    m_entities->emplace_back(breakable);
    m_entitiesMutex.unlock();

    // Load texture and add to new entities textures
    uint32_t texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(breakable).textureId;
    const uint8_t *texturePixels = m_graphicModule->getTexturePixels(texture);
    const Rte::Vec2<uint16_t> textureSize = m_graphicModule->getTextureSize(texture);

    std::vector<Rte::u8> pixelsVector(texturePixels, texturePixels + static_cast<ptrdiff_t>(textureSize.x * textureSize.y) * 4);

    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = textureSize;
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[breakable] = packedTexture;
    
    return (breakable);
}

Rte::Entity ServerApp::createBreakable(Rte::Vec2<float> pos, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size) {
    // Add breakable to the entities list
    Rte::Entity breakable = m_ecs->createEntity();
    // Load texture
    uint32_t breakableTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(breakableTexture, texture.data(), size))
        throw std::runtime_error("Failed to load texture from memory");

    uint32_t breakableMaterial = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(breakableMaterial, material.data(), size))
        throw std::runtime_error("Failed to load texture from memory");

    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(breakable, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(breakable, Rte::Graphic::Components::Sprite{.textureId = breakableTexture, .offset = {0, 0}, .layer = 0});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(breakable, Rte::BasicComponents::Transform{
        .position = {pos.x + static_cast<float>(m_graphicModule->getWindowSize().x) / 2, pos.y + static_cast<float>(m_graphicModule->getWindowSize().y) / 2},
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(breakable, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        m_graphicModule->getTexturePixels(breakableMaterial),
        m_graphicModule->getTextureSize(breakableMaterial),
        pos,
        0
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(breakable));

    // Add breakable to the entities list
    m_entitiesMutex.lock();
    m_entities->emplace_back(breakable);
    m_entitiesMutex.unlock();

    // Load texture and add to new entities textures
    const uint8_t *texturePixels = m_graphicModule->getTexturePixels(breakableTexture);
    const Rte::Vec2<uint16_t> textureSize = m_graphicModule->getTextureSize(breakableTexture);

    std::vector<Rte::u8> pixelsVector(texturePixels, texturePixels + static_cast<ptrdiff_t>(textureSize.x * textureSize.y) * 4);

    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = textureSize;
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[breakable] = packedTexture;

    return (breakable);
}

Rte::Entity ServerApp::createBreakable(Rte::Entity breakable, std::vector<Rte::u8> texture, std::vector<Rte::u8> material, Rte::Vec2<Rte::u16> size) {
    // Add breakable to the entities list
    Rte::Entity newBreakables = m_ecs->createEntity();
    // Load texture
    uint32_t breakableTexture = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(breakableTexture, texture.data(), size))
        throw std::runtime_error("Failed to load texture from memory");

    uint32_t breakableMaterial = m_graphicModule->createTexture();
    if (!m_graphicModule->loadTextureFromMemory(breakableMaterial, material.data(), size))
        throw std::runtime_error("Failed to load texture from memory");

    Rte::Vec2<float> pos = m_ecs->getComponent<Rte::BasicComponents::Transform>(breakable).position;

    // Add breakable components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(newBreakables, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(newBreakables, Rte::Graphic::Components::Sprite{.textureId = breakableTexture, .offset = {0, 0}, .layer = 0});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(newBreakables, Rte::BasicComponents::Transform{
        .position = pos,
        .scale = {8, 8},
        .rotation = 0
    });

    m_ecs->addComponent<Rte::Physics::Components::Physics>(newBreakables, Rte::Physics::Components::Physics{.rigidBody = m_physicsModule->createRigidBody(
        m_ecs->getComponent<Rte::Physics::Components::Physics>(breakable).rigidBody,
        m_graphicModule->getTexturePixels(breakableMaterial),
        m_graphicModule->getTextureSize(breakableMaterial)
    )});

    // Add breakable to the breakables list
    m_breakables.push_back(std::make_unique<Rte::Entity>(newBreakables));

    // Add breakable to the entities list
    m_entitiesMutex.lock();
    m_entities->emplace_back(newBreakables);
    m_entitiesMutex.unlock();

    // Load texture and add to new entities textures
    const uint8_t *texturePixels = m_graphicModule->getTexturePixels(breakableTexture);
    const Rte::Vec2<uint16_t> textureSize = m_graphicModule->getTextureSize(breakableTexture);

    std::vector<Rte::u8> pixelsVector(texturePixels, texturePixels + static_cast<ptrdiff_t>(textureSize.x * textureSize.y) * 4);

    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = textureSize;
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[newBreakables] = packedTexture;
 
    return (breakable);
}

void ServerApp::updateBreakables() {
    for (size_t i = 0; i < m_breakables.size(); i++) {
        // projectile collision
        for (size_t j = 0; j < m_projectiles.size(); j++) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_projectiles[j]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_breakables[i]).rigidBody)) {
                fractureBreakable({static_cast<unsigned short>(m_ecs->getComponent<Rte::BasicComponents::Transform>(*m_projectiles[j]).position.x + 1920 / 2), static_cast<unsigned short>(m_ecs->getComponent<Rte::BasicComponents::Transform>(*m_projectiles[j]).position.y + 1080 / 2)});
                destroyProjectile(*m_projectiles[j]);
                return;
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
}
