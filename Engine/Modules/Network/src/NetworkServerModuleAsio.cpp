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
#include <thread>
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

void Rte::Network::NetworkServerModuleAsio::deletePlayer(BasicComponents::UidComponents id, uint32_t playerId) {
    m_server->sendDeletePlayer(id, playerId);
}

// JB me send mtn que les entity qui ont gechan
// JB me dit mtn qq les entity sont deleted

void Rte::Network::NetworkServerModuleAsio::update() {
    m_server->update();
}

void Rte::Network::NetworkServerModuleAsio::sendUpdate() {
    if (m_server == nullptr) return;
    if (m_entities == nullptr) return;

    for (int i = 0; i < m_entities->size(); i++) {
        const BasicComponents::UidComponents uidComponent = m_ecs->getComponent<BasicComponents::UidComponents>(m_entities->at(i));
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(m_entities->at(i));
        auto it = std::find(m_alreadySentEntity.begin(), m_alreadySentEntity.end(), m_entities->at(i));

        if (it == m_alreadySentEntity.end()) {
            m_alreadySentEntity.push_back(m_entities->at(i));

            m_server->sendNewEntity(transformComponent, m_textures[m_entities->at(i)].pixels, m_textures[m_entities->at(i)].size, uidComponent);
        } else {
            m_server->sendUpdatedEntity(transformComponent, uidComponent);
        }

    }
}
