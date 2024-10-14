/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#include "NetworkServerModuleAsio.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "machin.hpp"

#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace Rte;

void Rte::Network::NetworkServerModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    m_buffer = new char[4096];

    m_ecs->addEventListener(LAMBDA_LISTENER(Events::MICRO_EVENT, [&](const Event& /* UNUSED */) {
        m_alreadySentEntity.clear();
    }));

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        exit(1);
    }
#endif

    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Error opening socket." << std::endl;
        exit(1);
    }

    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(m_port);

    if (bind(m_sockfd, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
        std::cerr << "Error binding socket." << std::endl;
        exit(1);
    }

    int flags = fcntl(m_sockfd, F_GETFL, 0);
    fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);

    m_clientLen = sizeof(m_clientAddr);

    m_thread = std::thread([&]() {
        fd_set readfds;

        while (true) {
            FD_ZERO(&readfds);
            FD_SET(m_sockfd, &readfds);

            struct timeval tv {
                .tv_sec = 100,
                .tv_usec = 0
            };

            int retval = select(m_sockfd + 1, &readfds, nullptr, nullptr, &tv);

            if (retval == -1) {
                std::cerr << "Error in select." << std::endl;
                continue;
            }
            if (retval == 0) {
                std::cout << "Timeout occurred! No data received." << std::endl;
                continue;
            }

            if (FD_ISSET(m_sockfd, &readfds)) {
                size_t receivedSize = receiveMessage();
                if (receivedSize < 0) {
                    std::cerr << "Error receiving data." << std::endl;
                    continue;
                }

                std::string message = m_buffer;
                switch (static_cast<MessageType>(std::stoi(message))) {
                    case MessageType::CONNECTION_REQUEST: {
                        std::cout << "Received connection request (message = \"" << message << "\")" << std::endl;
                        std::string connectionResponse = std::to_string(static_cast<uint32_t>(MessageType::CONNECTION_REPONSE)) + " " + std::to_string(m_lastClientId);
                        sendMessage(connectionResponse.c_str(), connectionResponse.size());
                        break;
                    }
                    case MessageType::INPUT: {
                        std::cout << "Received input (message = \"" << message << "\")" << std::endl;
                        break;
                    }
                    default:
                        std::cerr << "Unknown message type (message = \"" << message << "\")" << std::endl;
                        break;
                }
            }
        }
    });
}

size_t Rte::Network::NetworkServerModuleAsio::receiveMessage() {
    memset(m_buffer, 0, 4096);
    return recvfrom(m_sockfd, m_buffer, 4096, 0, (struct sockaddr *)&m_clientAddr, &m_clientLen);
}

void Rte::Network::NetworkServerModuleAsio::sendMessage(const char* message, int size) {
    int sentSize = sendto(m_sockfd, message, size, 0, (struct sockaddr *)&m_clientAddr, m_clientLen);
    if (sentSize < 0)
        std::cerr << "Error sending data." << std::endl;
}

void Rte::Network::NetworkServerModuleAsio::updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) {
    m_entities = entities;
}

void Rte::Network::NetworkServerModuleAsio::updateTexture(std::map<Entity, PackedTexture>& textures) {
    m_textures = textures;
}

void Rte::Network::NetworkServerModuleAsio::deleteEntity(BasicComponents::UidComponents id) {
}

void Rte::Network::NetworkServerModuleAsio::deletePlayer(BasicComponents::UidComponents id, uint32_t playerId) {
}

void Rte::Network::NetworkServerModuleAsio::update() {
}

void Rte::Network::NetworkServerModuleAsio::sendUpdate() {
}