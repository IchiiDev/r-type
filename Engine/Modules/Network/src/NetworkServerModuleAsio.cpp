/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#include "NetworkServerModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

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

void Rte::Network::NetworkServerModuleAsio::updateTexture(std::map<Entity, PackedTexture>& textures) {
    m_textures = textures;
}

void Rte::Network::NetworkServerModuleAsio::deleteEntity(BasicComponents::UidComponents id) {
    m_server->sendDeleteEntity(id);
}

// JB me send mtn que les entity qui ont gechan
// JB me dit mtn qq les entity sont deleted

void Rte::Network::NetworkServerModuleAsio::update() {
    m_server->update();

    Event event(Rte::Network::Events::INPUT);
    event.setParameter<PackedInput>(Rte::Network::Events::Params::INPUT, m_server->getCurrentInput());
    m_ecs->sendEvent(event);
}

void Rte::Network::NetworkServerModuleAsio::sendUpdate() {
    if (m_server == nullptr) return;
    if (m_entities == nullptr) return;

    for (auto& entity : *m_entities) {
        const BasicComponents::UidComponents uidComponent = m_ecs->getComponent<BasicComponents::UidComponents>(entity);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
        auto it = std::find(m_alreadySentEntity.begin(), m_alreadySentEntity.end(), entity);

        if (it == m_alreadySentEntity.end()) {
            m_alreadySentEntity.push_back(entity);

            m_server->sendNewEntity(transformComponent, m_textures[entity].pixels, m_textures[entity].size, uidComponent);
        } else {
            m_server->sendUpdatedEntity(transformComponent, uidComponent);
        }

    }
}
