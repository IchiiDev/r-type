/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/


#include "Rte/Network/NetworkModuleTypes.hpp"
#include "NetworkClientModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"

#include <memory>

using namespace Rte;

void Rte::Network::NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkClientModuleAsio::connect(const std::string& host, const unsigned int& port) {
    m_client = std::make_unique<CustomClient>();
	m_client->connect(host, port);
}

void Rte::Network::NetworkClientModuleAsio::updateInputs(PackedInput input) {
    m_input = input;
}

void Rte::Network::NetworkClientModuleAsio::update() {
    if (m_client == nullptr) return;

    if (!m_client->isConnected()) {
        m_ecs->sendEvent(Events::DISCONNECTED);
        m_client = nullptr;
    }

    if (!m_client->getReiceiveQueue().empty()) {
        auto msg = m_client->getReiceiveQueue().popFront().msg;

        switch (msg.header.id) {
            case Rte::Network::CustomMsgTypes::EntityCreated: {
                BasicComponents::UidComponents id{};
                BasicComponents::Transform transform;
                std::vector<u8> pixels;
                Vec2<u16> size{};

                msg >> size >> pixels >> transform >> id;

                PackedNewEntity packedNewEntity(id.uid, transform, pixels, size);

                Event event(Rte::Network::Events::ENTITY_CREATED);
                event.setParameter<PackedNewEntity>(Rte::Network::Events::Params::PACKED_NEW_ENTITY, packedNewEntity);
                break;
            } case Rte::Network::CustomMsgTypes::EntityUpdated: {
                BasicComponents::UidComponents id{};
                BasicComponents::Transform transform;

                msg >> transform >> id;

                PackedUpdateEntity packedUpdateEntity;
                packedUpdateEntity.id = id.uid;
                packedUpdateEntity.transform = transform;

                Event event(Rte::Network::Events::ENTITY_UPDATED);
                event.setParameter<PackedUpdateEntity>(Rte::Network::Events::Params::PACKED_UPDATE_ENTITY, packedUpdateEntity);
                break;
            }
            break;
        }
    }
}
