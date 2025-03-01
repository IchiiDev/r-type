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
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <vector>

namespace Rte::Network {
    enum class CustomMsgTypes : uint32_t {
	    EntityUpdated, // BasicComponents::UidComponents id <------> BasicComponets::Transform transform
        EntityCreated, // BasicComponents::UidComponents id <------> BasicComponets::Transform transform <------> std::vector<u8> pixels <------> Vec2<u16> size
        EntityDeleted, // BasicComponents::UidComponents id
        Input, // Packed Input
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
            CustomServer(uint16_t nPort, std::shared_ptr<Ecs>& ecs) : bnl::net::IServer<CustomMsgTypes>(nPort), m_ecs(ecs) {
            }

        public:
            void sendNewEntity(BasicComponents::Transform transform, const std::vector<u8>& pixels, Vec2<u16> size, BasicComponents::UidComponents uidComponent) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityCreated;

                std::array<u8, 250 * 250> pixelArray; // TODO: REMOVE THIS !!!!!!??????
                if (pixels.size() > 62500) {
                    std::cout << "Yohan nerf le gros cul de ton asteroid sinon je le send pas ! " << pixels.size() << std::endl;
                    return;
                }
                memcpy(pixelArray.data(), pixels.data(), pixels.size());
                msg << uidComponent << transform << pixelArray << size;

                messageAllClient(msg);
            }

            void sendUpdatedEntity(BasicComponents::Transform transform, BasicComponents::UidComponents uidComponent) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityUpdated;

                msg << transform << uidComponent;

                messageAllClient(msg);
            }

            void sendDeleteEntity(BasicComponents::UidComponents uidComponent) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityDeleted;

                msg << uidComponent;

                messageAllClient(msg);
            }

            void checkClientTimeouts() {
                auto now = std::chrono::steady_clock::now();
                std::vector<std::shared_ptr<bnl::net::Connection<CustomMsgTypes>>> clientsToRemove;

                for (auto & it : m_lastMessageTime) {
                    std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client = it.first;
                    auto lastMessageTime = it.second;

                    // Check if the time since the last message exceeds the timeout threshold
                    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastMessageTime).count() > 1) {
                        // Add client to removal list and disconnect
                        std::cout << "Disconnecting client " << client->getId() << " due to inactivity." << std::endl;
                        client->disconnect();
                    }
                }
            }

            /*
            void sendString() {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::TestString;

                std::string testString = "Hello World!";

                msg << testString;

                messageAllClient(msg);
            }

            void sendVector() {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::TestVector;

                std::vector<uint32_t> testVec;
                testVec.push_back(69);
                testVec.push_back(420);
                testVec.push_back(1101);

                msg << testVec;

                messageAllClient(msg);
            } Exemple of sending vectors and strings */

            void sendDeletePlayer(BasicComponents::UidComponents uidComponent, uint32_t playerId) {
                bnl::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::EntityDeleted;

                msg << uidComponent;

                for (auto& client : m_connectionsQueue) {
                    if (client && client->isConnected() && client->getId() == playerId) {
                        messageClient(client, msg);
                        break;
                    }
                }
            }

        public:
            PackedInput getCurrentInput() { return m_currentInput; }

        protected:
        bool onClientConnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
            bool result = m_connectionsQueue.size() <= 8;

            if (result) {
                // Track the new client with the current time as the last message time
                m_lastMessageTime[client] = std::chrono::steady_clock::now();

                Event event(Rte::Network::Events::PLAYER_CREATED);
                event.setParameter<uint32_t>(Events::Params::PLAYER_ID, client->getId());
                m_ecs->sendEvent(event);

                event = Event(Events::MICRO_EVENT);
                m_ecs->sendEvent(event);
            }

            return result;
        }

        void onClientDisconnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
            Event event(Rte::Network::Events::PLAYER_DELETED);
            event.setParameter<uint32_t>(Events::Params::PLAYER_ID, client->getId());
            m_ecs->sendEvent(event);

            // Remove client from the last message time map if they disconnect
            m_lastMessageTime.erase(client);
        }

        void onMessageReceived(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client, bnl::net::message<CustomMsgTypes>& msg) override {
            // Update the last message time for the client
            m_lastMessageTime[client] = std::chrono::steady_clock::now();

            switch (msg.header.id) {
                case CustomMsgTypes::Input: {
                    PackedInput input;
                    msg >> input;
                    m_currentInput = input;

                    Event event(Rte::Network::Events::INPUT);
                    event.setParameter<PackedInput>(Rte::Network::Events::Params::INPUT, input);
                    event.setParameter<uint32_t>(Rte::Network::Events::Params::PLAYER_ID, client->getId());
                    m_ecs->sendEvent(event);
                    break;
                }
                case CustomMsgTypes::EntityUpdated:
                case CustomMsgTypes::EntityCreated:
                case CustomMsgTypes::EntityDeleted:
                    break;
            }
        }
        private:
            std::shared_ptr<Ecs> m_ecs;
            PackedInput m_currentInput;
            std::unordered_map<std::shared_ptr<bnl::net::Connection<CustomMsgTypes>>, std::chrono::steady_clock::time_point> m_lastMessageTime;
    };
} // namespace Rte::Network
