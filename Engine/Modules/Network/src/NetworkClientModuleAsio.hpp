/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/

#pragma once

#include "Rte/Network/NetworkModuleClient.hpp"
#include "Rte/Network/NetworkModule.hpp"

#include <array>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace Rte::Network {
    class NetworkClientModuleAsio : public NetworkModuleClient {
        public:
            NetworkClientModuleAsio() = default;
            ~NetworkClientModuleAsio() = default;

        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

        public:
            void connect(const char *host, uint32_t port) override;
            void updateInputs(PackedInput input) override;
            void sendUpdate() override;

        private:
            void sendMessageToServer(const char *message, int size);
            size_t receiveMessageFromServer();

        private:
            PackedInput m_input;
            bool m_synAck = false;
            std::shared_ptr<Ecs> m_ecs;

            int m_socket;
            struct sockaddr_in m_serverAddr;

            char *m_buffer;
            int m_clientId = 0;
    };
} // namespace Rte::Network
