/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#include "NetworkServerModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"
#include <memory>

using namespace Rte;

IModule *createModule() {
    return new Rte::Network::NetworkServerModuleAsio(); // NOLINT(cppcoreguidelines-owning-memory)
}

void Rte::Network::NetworkServerModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkServerModuleAsio::start(const unsigned int port) {
    m_server = std::make_unique<CustomServer>(port);
	m_server->start();
}

void Rte::Network::NetworkServerModuleAsio::update() {
    if (m_server == nullptr) return;
    
    m_server->update();
}
