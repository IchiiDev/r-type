/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleImpl
*/

#pragma once

#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace Rte::Network {
    enum class CustomMsgTypes : uint32_t {
	    EntityUpdated, // BasicComponents::UidComponents id <------> BasicComponets::Transform transform
        EntityCreated, // BasicComponents::UidComponents id <------> BasicComponets::Transform transform <------> std::vector<u8> pixels <------> Vec2<u16> size
        Input // Packed Input
    };

    class CustomClient : public bnl::net::IClient<CustomMsgTypes> {
        public:
            void sendInputs(PackedInput input) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::Input;

                msg << input;

                send(msg);
            }
    };

    class CustomServer : public bnl::net::IServer<CustomMsgTypes> {
        public:
            CustomServer(uint16_t nPort, std::shared_ptr<Ecs>& ecs) : bnl::net::IServer<CustomMsgTypes>(nPort), m_ecs(ecs) {}

        public:
            void sendNewEntity(BasicComponents::Transform transform, const u8 *pixels, Vec2<u16> size, BasicComponents::UidComponents uidComponent) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityCreated;

                std::vector<u8> pixelsInVector(pixels, pixels + size.x * size.y * 4);
                msg << uidComponent << transform << pixelsInVector << size;

                messageAllClient(msg);
            }

            void sendUpdatedEntity(BasicComponents::Transform transform, BasicComponents::UidComponents uidComponent) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityUpdated;

                msg << transform << uidComponent;

                messageAllClient(msg);
            }

        protected:
            bool onClientConnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
                bool result = m_connectionsQueue.size() <= 8;

                if (result) {
                    Event event(Rte::Network::Events::PLAYER_CREATED);
                    m_ecs->sendEvent(event);
                }

                return result;
            }

            void onClientDisconnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
            }

            void onMessageReceived (std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client, bnl::net::message<CustomMsgTypes>& msg) override {
                switch (msg.header.id) {
                    case CustomMsgTypes::Input: {
                        PackedInput input;

                        msg >> input;

                        Event event(Rte::Network::Events::INPUT);
                        event.setParameter<PackedInput>(Rte::Network::Events::Params::PACKED_UPDATE_ENTITY, input);
                        m_ecs->sendEvent(event);
                        break;
                    } break;
                }
            }
        private:
            std::shared_ptr<Ecs> m_ecs;
    };
} // namespace Rte::Network
