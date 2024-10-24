#include "ServerApp.hpp"

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Components.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "Rte/Physics/Components.hpp"
#include <cstdlib>
#include <memory>

void ServerApp::createPowerup(Rte::Vec2<float> pos) {
    // Add powerup to the entities list
    Rte::Entity powerup = m_ecs->createEntity();
    // Load texture
    const std::shared_ptr<Rte::Graphic::Texture> powerupTexture = m_graphicModule->createTexture();
    powerupTexture->loadFromFile("../assets/powerup.png");
    
    // Add powerup components
    m_ecs->addComponent<Rte::BasicComponents::UidComponents>(powerup, Rte::BasicComponents::UidComponents{m_currentUid++});
    m_ecs->addComponent<Rte::Graphic::Components::Sprite>(powerup, Rte::Graphic::Components::Sprite{powerupTexture});
    m_ecs->addComponent<Rte::BasicComponents::Transform>(powerup, Rte::BasicComponents::Transform{
        .position = {pos.x + static_cast<float>(m_graphicModule->getWindowSize().x) / 2, pos.y + static_cast<float>(m_graphicModule->getWindowSize().y) / 2},
        .scale = {3, 3},
        .rotation = 0
    });
    m_ecs->addComponent<Rte::Physics::Components::Physics>(powerup, Rte::Physics::Components::Physics{.shapeBody = m_physicsModule->createShapeBody(
        {32 * 2, 32 * 2}, 0.1, 0,
        m_ecs->getComponent<Rte::BasicComponents::Transform>(powerup).position, 0, false, false,
        Rte::Physics::ShapeType::RECTANGLE
    )});

    // Add powerup to the powerups list
    m_powerups.push_back(std::make_unique<Rte::Entity>(powerup));

    // Add powerup to the entities list
    m_entities->emplace_back(powerup);

    // Load texture and add to new entities textures
    auto texture = m_ecs->getComponent<Rte::Graphic::Components::Sprite>(powerup).texture;
    std::vector<Rte::u8> pixelsVector(texture->getPixels(), texture->getPixels() + static_cast<ptrdiff_t>(texture->getSize().x * texture->getSize().y) * 4);
    
    Rte::Network::PackedTexture packedTexture{};
    packedTexture.size = texture->getSize();
    packedTexture.pixels = pixelsVector;
    m_newEntitiesTextures[powerup] = packedTexture;
}

void ServerApp::updatePowerups() {
    for (size_t i = 0; i < m_powerups.size(); i++) {
        for (auto& [playerId, player] : m_players) {
            if (m_physicsModule->colliding(m_ecs->getComponent<Rte::Physics::Components::Physics>(*m_powerups[i]).shapeBody, m_ecs->getComponent<Rte::Physics::Components::Physics>(player->getEntity()).shapeBody)) {
                player->heal(40);
                destroyPowerup(*m_powerups[i]);
                return;
            }
        }
    }
}

void ServerApp::destroyPowerup(const Rte::Entity& powerup) {
    const Rte::BasicComponents::UidComponents uid = m_ecs->getComponent<Rte::BasicComponents::UidComponents>(powerup);
    m_ecs->getComponent<Rte::Physics::Components::Physics>(powerup).shapeBody.reset();
    for (size_t j = 0; j < m_entities->size(); j++) {
        if (m_ecs->getComponent<Rte::BasicComponents::UidComponents>((*m_entities)[j]).uid == uid.uid) {
            m_entities->erase(std::next(m_entities->begin(), static_cast<std::ptrdiff_t>(j)));
            break;
        }
    }
    m_ecs->destroyEntity(powerup);
    for (size_t i = 0; i < m_powerups.size(); i++)
        if (*m_powerups[i] == powerup)
            m_powerups.erase(std::next(m_powerups.begin(), static_cast<std::ptrdiff_t>(i)));
    m_networkModuleServer->deleteEntity(uid);
    m_networkModuleServer->updateEntity(m_entities);
}
