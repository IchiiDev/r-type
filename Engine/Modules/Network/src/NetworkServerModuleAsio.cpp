/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#include "NetworkServerModuleAsio.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Ecs/Event.hpp"
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
#include <fcntl.h>

using namespace Rte;

void Rte::Network::NetworkServerModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    m_buffer = new char[100000];

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

    #if defined(__linux__)
        int flags = fcntl(m_sockfd, F_GETFL, 0);
        fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);
    #elif defined(_WIN32)
        u_long mode = 1;
        ioctlsocket(m_sockfd, FIONBIO, &mode);
    #endif

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

                        std::string connectionResponse = std::to_string(static_cast<uint32_t>(MessageType::CONNECTION_REPONSE)) + " " + std::to_string(m_lastClientId++);
                        sendMessage(connectionResponse.c_str(), connectionResponse.size());

                        Event event(Events::PLAYER_CREATED);
                        event.setParameter<uint32_t>(Events::Params::PLAYER_ID, m_lastClientId - 1);
                        m_ecs->sendEvent(event);

                        Event microEvent(Events::MICRO_EVENT);
                        m_ecs->sendEvent(microEvent);
                        break;
                    }
                    case MessageType::INPUT: {
                        uint32_t playerId = std::stoi(message.c_str() + 2);

                        const Rte::Network::PackedInput packedInput = {
                            .moveUp = message[4] == '1',
                            .moveDown = message[5] == '1',
                            .moveLeft = message[6] == '1',
                            .moveRight = message[7] == '1',
                            .shoot = message[8] == '1'
                        };

                        Event event(Events::INPUT);
                        event.setParameter<uint32_t>(Events::Params::PLAYER_ID, playerId);
                        event.setParameter<Rte::Network::PackedInput>(Events::Params::INPUT, packedInput);
                        m_ecs->sendEvent(event);
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
    memset(m_buffer, 0, 100000);
    return recvfrom(m_sockfd, m_buffer, 100000, 0, (struct sockaddr *)&m_clientAddr, &m_clientLen);
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
    // Mesage structure = "<MessageType> <EntityId>"
    std::vector<char> message;
    message.push_back(std::to_string(static_cast<uint32_t>(MessageType::ENTITY_DELETED)).c_str()[0]);   // Message type
    message.push_back(' ');

    std::string entityId = std::to_string(id.uid);
    message.insert(message.end(), entityId.begin(), entityId.end());

    sendMessage(message.data(), message.size());
}

void Rte::Network::NetworkServerModuleAsio::deletePlayer(BasicComponents::UidComponents id, uint32_t playerId) {
}

void Rte::Network::NetworkServerModuleAsio::update() {
}

void Rte::Network::NetworkServerModuleAsio::sendUpdate() {
    for (const Entity entity : *m_entities) {
        if (std::find(m_alreadySentEntity.begin(), m_alreadySentEntity.end(), entity) == m_alreadySentEntity.end()) {
            // Entity created
            m_alreadySentEntity.push_back(entity);

            const PackedNewEntity packedNewEntity = {
                .id = entity,
                .transform = m_ecs->getComponent<BasicComponents::Transform>(entity),
                .pixels = m_textures[entity].pixels,
                .size = m_textures[entity].size
            };

            for (int i = 0; i < m_lastClientId; i++) {
                std::vector<char> message;
                message.push_back(std::to_string(static_cast<uint32_t>(MessageType::ENTITY_CREATED)).c_str()[0]);   // Message type
                message.push_back(' ');

                std::string clientId = std::to_string(i);
                message.insert(message.end(), clientId.begin(), clientId.end());
                message.push_back(' ');

                std::string entityId = std::to_string(packedNewEntity.id);
                message.insert(message.end(), entityId.begin(), entityId.end());
                message.push_back(' ');

                std::string transformX = std::to_string(packedNewEntity.transform.position.x);
                message.insert(message.end(), transformX.begin(), transformX.end());
                message.push_back(' ');

                std::string transformY = std::to_string(packedNewEntity.transform.position.y);
                message.insert(message.end(), transformY.begin(), transformY.end());
                message.push_back(' ');

                std::string transformSizeX = std::to_string(packedNewEntity.transform.scale.x);
                message.insert(message.end(), transformSizeX.begin(), transformSizeX.end());
                message.push_back(' ');

                std::string transformSizeY = std::to_string(packedNewEntity.transform.scale.y);
                message.insert(message.end(), transformSizeY.begin(), transformSizeY.end());
                message.push_back(' ');

                std::string transformRotation = std::to_string(packedNewEntity.transform.rotation);
                message.insert(message.end(), transformRotation.begin(), transformRotation.end());
                message.push_back(' ');

                std::string textureSizeX = std::to_string(packedNewEntity.size.x);
                message.insert(message.end(), textureSizeX.begin(), textureSizeX.end());
                message.push_back(' ');

                std::string textureSizeY = std::to_string(packedNewEntity.size.y);
                message.insert(message.end(), textureSizeY.begin(), textureSizeY.end());
                message.push_back(' ');

                std::vector<char> pixelsAsChar(packedNewEntity.pixels.size());
                for (int i = 0; i < packedNewEntity.pixels.size(); i++)
                    pixelsAsChar[i] = static_cast<char>(packedNewEntity.pixels[i]);
                message.insert(message.end(), pixelsAsChar.begin(), pixelsAsChar.end());

                sendMessage(message.data(), message.size());
            }

            std::cout << "Entity created" << std::endl;
        } else {
            // The string follow the format: "<MessageType> <PlayedId> <EntityId> <Transform.pos.x> <Transform.pos.y> <Transform.size.x> <Transform.size.y> <Transform.rotation>"

            const PackedUpdateEntity packedUpdateEntity = {
                .id = entity,
                .transform = m_ecs->getComponent<BasicComponents::Transform>(entity)
            };

            for (int i = 0; i < m_lastClientId; i++) {
                std::vector<char> message;
                message.push_back(std::to_string(static_cast<uint32_t>(MessageType::ENTITY_UPDATED)).c_str()[0]);   // Message type
                message.push_back(' ');

                std::string clientId = std::to_string(i);
                message.insert(message.end(), clientId.begin(), clientId.end());
                message.push_back(' ');

                std::string entityId = std::to_string(packedUpdateEntity.id);
                message.insert(message.end(), entityId.begin(), entityId.end());
                message.push_back(' ');

                std::string transformX = std::to_string(packedUpdateEntity.transform.position.x);
                message.insert(message.end(), transformX.begin(), transformX.end());
                message.push_back(' ');

                std::string transformY = std::to_string(packedUpdateEntity.transform.position.y);
                message.insert(message.end(), transformY.begin(), transformY.end());
                message.push_back(' ');

                std::string transformSizeX = std::to_string(packedUpdateEntity.transform.scale.x);
                message.insert(message.end(), transformSizeX.begin(), transformSizeX.end());
                message.push_back(' ');

                std::string transformSizeY = std::to_string(packedUpdateEntity.transform.scale.y);
                message.insert(message.end(), transformSizeY.begin(), transformSizeY.end());
                message.push_back(' ');

                std::string transformRotation = std::to_string(packedUpdateEntity.transform.rotation);
                message.insert(message.end(), transformRotation.begin(), transformRotation.end());

                sendMessage(message.data(), message.size());
            }
        }
    }
}
