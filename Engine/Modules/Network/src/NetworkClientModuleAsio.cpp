/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/


#include "Rte/Ecs/Event.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "NetworkClientModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"

#include <array>
#include <iostream>
#include <iterator>
#include <memory>

using namespace Rte;

void Rte::Network::NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkClientModuleAsio::connect(const std::string& host, const unsigned int& port) {
    m_client = std::make_unique<CustomClient>();
	m_client->connect(host, port);

    connectUdp(host, port, 10);
}

void Rte::Network::NetworkClientModuleAsio::connectUdp(const std::string& host, const unsigned int& port, const unsigned int maxTry) {
    bnl::net::message<CustomMsgTypes> msgSyn;
    msgSyn.header.id = CustomMsgTypes::SYN;
    m_client->send(msgSyn);

    while (!m_synAck) {
        update();
    }

    bnl::net::message<CustomMsgTypes> msgAck;
    msgAck.header.id = CustomMsgTypes::ACK;
    m_client->send(msgAck);

    std::cout << "udp connection ok on client" << std::endl;
}

void Rte::Network::NetworkClientModuleAsio::updateInputs(PackedInput input) {
    m_input = input;
}

void Rte::Network::NetworkClientModuleAsio::sendUpdate() {
    if (m_client != nullptr)
        m_client->sendInputs(m_input);
}

void Rte::Network::NetworkClientModuleAsio::update() {
    if (m_client == nullptr) return;

    if (!m_client->isConnected()) {
        m_ecs->sendEvent(Events::DISCONNECTED);
        m_client = nullptr;
        return;
    }


    if (!m_client->getReiceiveQueue().empty()) {
        auto msg = m_client->getReiceiveQueue().popFront().msg;

        switch (msg.header.id) {
            case Rte::Network::CustomMsgTypes::EntityCreated: {
                BasicComponents::UidComponents id{};
                BasicComponents::Transform transform;
                std::array<u8, 1000 * 1000> pixels; // TODO: REMOVE THIS !!!!!!??????
                Vec2<u16> size{};

                msg >> size >> pixels >> transform >> id;

                PackedNewEntity packedNewEntity = {
                    .id = id.uid,
                    .transform = transform,
                    .pixels = std::vector<u8>(pixels.begin(), pixels.end()),
                    .size = size
                };

                Event event(Rte::Network::Events::ENTITY_CREATED);
                event.setParameter<PackedNewEntity>(Rte::Network::Events::Params::PACKED_NEW_ENTITY, packedNewEntity);
                m_ecs->sendEvent(event);
                break;
            } case Rte::Network::CustomMsgTypes::EntityUpdated: {
                BasicComponents::UidComponents id{};
                BasicComponents::Transform transform{};

                msg >> id >> transform;

                const PackedUpdateEntity packedUpdateEntity{
                    .id = id.uid,
                    .transform = transform
                };

                Event event(Rte::Network::Events::ENTITY_UPDATED);
                event.setParameter<PackedUpdateEntity>(Rte::Network::Events::Params::PACKED_UPDATE_ENTITY, packedUpdateEntity);
                m_ecs->sendEvent(event);

                break;
            } case Rte::Network::CustomMsgTypes::EntityDeleted: {
                BasicComponents::UidComponents id{};

                msg >> id;

                Event event(Rte::Network::Events::ENTITY_DELETED);
                event.setParameter<BasicComponents::UidComponents>(Rte::Network::Events::Params::ENTITY_ID, id);
                m_ecs->sendEvent(event);
            } case Rte::Network::CustomMsgTypes::SYN_ACK: {
                m_synAck = true;
                break;
            }
            break;
        }
    }
}
