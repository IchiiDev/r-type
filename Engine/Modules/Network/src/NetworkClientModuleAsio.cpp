/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/


#include "NetworkClientModuleAsio.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"
#include "machin.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

using namespace Rte::Network;

void NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    m_buffer = new char[100000];
}

NetworkClientModuleAsio::~NetworkClientModuleAsio() {
    m_running = false;
    m_receiveThread.join();

    #ifdef _WIN32
        closesocket(m_socket);
        WSACleanup();
    #else
        close(m_socket);
    #endif
}

size_t NetworkClientModuleAsio::receiveMessageFromServer() {
    memset(m_buffer, 0, 100000);
    return recvfrom(m_socket, m_buffer, 100000, 0, nullptr, nullptr);
}

void NetworkClientModuleAsio::sendMessageToServer(const char *message, int size) {
    int sentSize = sendto(m_socket, message, size, 0, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr));
    if (sentSize < 0)
        std::cerr << "Error sending data." << std::endl;
}

void NetworkClientModuleAsio::connect(const char *host, uint32_t port) {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            exit(1);
        }
    #endif

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_socket < 0) {
        std::cerr << "Error opening socket." << std::endl;
        exit(1);
    }

    memset(&m_serverAddr, 0, sizeof(m_serverAddr));
    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host, &m_serverAddr.sin_addr);

    std::string connectMessage = std::to_string(static_cast<uint32_t>(MessageType::CONNECTION_REQUEST));
    sendMessageToServer(connectMessage.c_str(), connectMessage.size());
    receiveMessageFromServer();
    m_clientId = std::stoi(m_buffer + 2);
    std::cout << "Received id: " << m_clientId << std::endl;

    m_receiveThread = std::thread([&]() {
        while (m_running) {
            size_t receivedSize = receiveMessageFromServer();
            if (receivedSize < 0) {
                std::cerr << "Error receiving data." << std::endl;
                continue;
            }

            switch (static_cast<MessageType>(std::stoi(m_buffer))) {
                case MessageType::ENTITY_CREATED: {
                    std::cout << "Received entity created message (message = \"" << m_buffer << "\")" << std::endl;
                    // The string follow the format: "<MessageType> <PlayerId> <EntityId> <Transform.pos.x> <Transform.pos.y> <Transform.size.x> <Transform.size.y> <Transform.rotation> <Texture.size.x> <Texture.size.y> <Pixels>"

                    std::istringstream iss(m_buffer);
                    std::string token;
                    std::vector<std::string> tokens;
                    while (std::getline(iss, token, ' ') && tokens.size() < 11)
                        tokens.push_back(token);

                    uint32_t playerId = std::stoi(tokens[1]);
                    if (playerId != m_clientId)
                        break;

                    uint32_t entityId = std::stoi(tokens[2]);
                    float transformPosX = std::stof(tokens[3]);
                    float transformPosY = std::stof(tokens[4]);
                    float transformSizeX = std::stof(tokens[5]);
                    float transformSizeY = std::stof(tokens[6]);
                    float transformRotation = std::stof(tokens[7]);
                    uint32_t textureSizeX = std::stoi(tokens[8]);
                    uint32_t textureSizeY = std::stoi(tokens[9]);

                    PackedNewEntity newEntity{};
                    newEntity.id = entityId;
                    newEntity.transform.position.x = transformPosX;
                    newEntity.transform.position.y = transformPosY;
                    newEntity.transform.scale.x = transformSizeX;
                    newEntity.transform.scale.y = transformSizeY;
                    newEntity.transform.rotation = transformRotation;
                    newEntity.size.x = textureSizeX;
                    newEntity.size.y = textureSizeY;

                    size_t pixelOffset = tokens[9].size();
                    newEntity.pixels.resize(textureSizeX * textureSizeY * 4);
                    for (int i = 0; i < textureSizeX * textureSizeY * 4; i++)
                        newEntity.pixels[i] = static_cast<uint8_t>(m_buffer[pixelOffset + i]);

                    Event event(Events::ENTITY_CREATED);
                    event.setParameter<PackedNewEntity>(Events::Params::PACKED_NEW_ENTITY, newEntity);
                    m_ecs->sendEvent(event);

                    break;
                }
                case MessageType::ENTITY_UPDATED: {
                    // The string follow the format: "<MessageType> <PlayedId> <EntityId> <Transform.pos.x> <Transform.pos.y> <Transform.size.x> <Transform.size.y> <Transform.rotation>"
                    std::istringstream iss(m_buffer);
                    std::string token;
                    std::vector<std::string> tokens;
                    while (std::getline(iss, token, ' ') && tokens.size() < 8)
                        tokens.push_back(token);

                    uint32_t playerId = std::stoi(tokens[1]);
                    if (playerId != m_clientId)
                        break;

                    uint32_t entityId = std::stoi(tokens[2]);
                    float transformPosX = std::stof(tokens[3]);
                    float transformPosY = std::stof(tokens[4]);
                    float transformSizeX = std::stof(tokens[5]);
                    float transformSizeY = std::stof(tokens[6]);
                    float transformRotation = std::stof(tokens[7]);

                    PackedUpdateEntity updateEntity{};
                    updateEntity.id = entityId;
                    updateEntity.transform.position.x = transformPosX;
                    updateEntity.transform.position.y = transformPosY;
                    updateEntity.transform.scale.x = transformSizeX;
                    updateEntity.transform.scale.y = transformSizeY;
                    updateEntity.transform.rotation = transformRotation;

                    Event event(Events::ENTITY_UPDATED);
                    event.setParameter<PackedUpdateEntity>(Events::Params::PACKED_UPDATE_ENTITY, updateEntity);
                    m_ecs->sendEvent(event);
                    break;
                }
                default:
                    std::cout << "Unknown message type (message = \"" << m_buffer << "\")" << std::endl;
                    break;
            }
        }
    });
}

void NetworkClientModuleAsio::updateInputs(PackedInput input) {
    std::string message = std::to_string(static_cast<uint32_t>(MessageType::INPUT)) + " " + std::to_string(m_clientId) + " " + std::to_string(input.moveUp) + std::to_string(input.moveDown) + std::to_string(input.moveLeft) + std::to_string(input.moveRight) + std::to_string(input.shoot);
    sendMessageToServer(message.c_str(), message.size());
}

void NetworkClientModuleAsio::sendUpdate() {
}

void NetworkClientModuleAsio::update() {
}
