/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#include "NetworkServerModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Graphic/Components.hpp"
#include <memory>

using namespace Rte;

void Rte::Network::NetworkServerModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkServerModuleAsio::start(const unsigned int port) {
    m_server = std::make_unique<CustomServer>(port, m_ecs);
	m_server->start();
}

void Rte::Network::NetworkServerModuleAsio::updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) {
    m_entities = entities;
}

void Rte::Network::NetworkServerModuleAsio::update() {
    if (m_server == nullptr) return;
    if (m_entities == nullptr) return;

    for (auto& entity : *m_entities) {
        const BasicComponents::UidComponents uidComponent = m_ecs->getComponent<BasicComponents::UidComponents>(entity);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
        auto it = std::find(m_alreadySentEntity.begin(), m_alreadySentEntity.end(), entity);

        if (it != m_alreadySentEntity.end()) {
            m_alreadySentEntity.push_back(entity);

            Graphic::Components::Sprite spriteComponent = m_ecs->getComponent<Graphic::Components::Sprite>(entity);
            std::vector<u8> texturePixels = spriteComponent.texture->getPixels();
            Vec2<u16> textureSize = spriteComponent.texture->getSize();

            m_server->sendNewEntity(transformComponent, texturePixels, textureSize, uidComponent);
        } else {
            m_server->sendUpdatedEntity(transformComponent, uidComponent);
        }
    }
}
