/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/

#include "NetworkClientModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Network/NetworkModuleClient.hpp"
#include <memory>

using namespace Rte;

Rte::IModule *createModule() {
    return new Rte::Network::NetworkClientModuleAsio(); // NOLINT(cppcoreguidelines-owning-memory)
}

void Rte::Network::NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkClientModuleAsio::connect(const std::string& host, const unsigned int& port) {
    m_client = std::make_unique<CustomClient>();
	m_client->connect(host, port);
}

void Rte::Network::NetworkClientModuleAsio::update() {
    if (m_client == nullptr) return;

    if (!m_client->isConnected()) {
        m_ecs->sendEvent(Events::Network::DISCONNECTED);
        m_client = nullptr;
    }
}
