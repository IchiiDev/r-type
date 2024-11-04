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
#include <cstring>
#include <map>
#include <memory>
#include <ostream>
#include <sys/types.h>
#include <vector>

using namespace Rte;

void Rte::Network::NetworkServerModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;

    m_ecs->addEventListener(LAMBDA_LISTENER(Events::MICRO_EVENT, [&](const Event& /* UNUSED */) {
        m_alreadySentEntity.clear();
    }));
}

void Rte::Network::NetworkServerModuleAsio::start(const uint16_t port) {
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

void Rte::Network::NetworkServerModuleAsio::update() {
    m_server->update();
    m_server->checkClientTimeouts();
}

void Rte::Network::NetworkServerModuleAsio::sendUpdate() {
    if (m_server == nullptr) return;
    if (m_entities == nullptr) return;

    for (unsigned int i : *m_entities) {
        const BasicComponents::UidComponents uidComponent = m_ecs->getComponent<BasicComponents::UidComponents>(i);
        const BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(i);
        auto it = std::find(m_alreadySentEntity.begin(), m_alreadySentEntity.end(), i);

        if (it == m_alreadySentEntity.end()) {
            m_alreadySentEntity.push_back(i);

            m_server->sendNewEntity(transformComponent, m_textures[i].pixels, m_textures[i].size, uidComponent);
        } else {
            m_server->sendUpdatedEntity(transformComponent, uidComponent);
        }
    }
}
