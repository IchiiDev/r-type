/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/


#include "Rte/Ecs/Event.hpp"
#include "NetworkClientModuleAsio.hpp"
#include "NetworkModuleImpl.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <array>
#include <memory>
#include <vector>

using namespace Rte;

void Rte::Network::NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
}

void Rte::Network::NetworkClientModuleAsio::connect(const std::string& host, const uint16_t& port) {
    bnl::net::message<CustomMsgTypes> msg;
    msg.header.id = CustomMsgTypes(int(-1));


    m_client = std::make_unique<CustomClient>();
	m_client->connect(host, port);
    m_client->send(msg);
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
                std::array<u8, 250 * 250> pixels; // TODO: REMOVE THIS !!!!!!??????
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

                break;
            }
            break;
        }
    }
}
