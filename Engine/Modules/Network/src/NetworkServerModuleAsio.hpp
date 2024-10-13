/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#pragma once

#include "Rte/Network/NetworkModuleServer.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <sys/types.h>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

namespace Rte::Network {

    class NetworkServerModuleAsio : public NetworkModuleServer {
        public:
            NetworkServerModuleAsio() = default;
            ~NetworkServerModuleAsio() override = default;

            NetworkServerModuleAsio(const NetworkServerModuleAsio& other) = delete;
            NetworkServerModuleAsio& operator=(const NetworkServerModuleAsio& other) = delete;

            NetworkServerModuleAsio(NetworkServerModuleAsio&& other) noexcept = delete;
            NetworkServerModuleAsio& operator=(NetworkServerModuleAsio&& other) noexcept = delete;

        public: // Module methods
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

        public: // Server methods
            void updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) override;
            void updateTexture(std::map<Entity, PackedTexture>& textures) override;
            void sendUpdate() override;
            void deleteEntity(BasicComponents::UidComponents id) override;
            void deletePlayer(BasicComponents::UidComponents id, uint32_t playerId) override;

        private:
            void sendMessage(const char* message, int size);
            size_t receiveMessage();

        private:
            std::shared_ptr<Ecs> m_ecs;

            std::vector<Entity> m_alreadySentEntity;
            std::map<Entity, PackedTexture> m_textures;
            std::shared_ptr<std::vector<Entity>> m_entities = nullptr;

            int m_port = 4242;
            int m_sockfd;
            struct sockaddr_in m_serverAddr;
            struct sockaddr_in m_clientAddr;
            socklen_t m_clientLen;

            std::thread m_thread;

            char *m_buffer;
            uint32_t m_lastClientId = 0;
            std::vector<uint32_t> m_clientIds;
    };

} // namespace Rte::Network
