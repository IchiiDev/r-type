/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleImpl
*/

#pragma once

#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"

#include <cstdint>

namespace Rte::Network {
    enum class CustomMsgTypes : uint32_t {
	    ServerAccept,
	    ServerDeny,
	    ServerPing,
	    MessageAll,
	    ServerMessage,
    };

    class CustomClient : public bnl::net::IClient<CustomMsgTypes> {
        
    };

    class CustomServer : public bnl::net::IServer<CustomMsgTypes> {
        public:
            CustomServer(uint16_t nPort) : bnl::net::IServer<CustomMsgTypes>(nPort) {}

        protected:
            bool onClientConnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
                return true;
            }

            // Called when a client appears to have disconnected
            void onClientDisconnect(std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client) override {
            }

            // Called when a message arrives
            void onMessageReceived (std::shared_ptr<bnl::net::Connection<CustomMsgTypes>> client, bnl::net::message<CustomMsgTypes>& msg) override {
                switch (msg.header.id) {
                    break;
                }
            }
    };
} // namespace Rte::Network
