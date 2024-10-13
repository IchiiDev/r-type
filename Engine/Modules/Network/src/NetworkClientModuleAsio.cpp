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
#include <string>

using namespace Rte::Network;

void NetworkClientModuleAsio::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;
    m_buffer = new char[4096];
}

size_t NetworkClientModuleAsio::receiveMessageFromServer() {
    memset(m_buffer, 0, 4096);
    return recvfrom(m_socket, m_buffer, 4096, 0, nullptr, nullptr);
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
    m_clientId = std::stoi(m_buffer);
    std::cout << "Received id: " << m_clientId << std::endl;
}

void NetworkClientModuleAsio::updateInputs(PackedInput input) {
}

void NetworkClientModuleAsio::sendUpdate() {
}

void NetworkClientModuleAsio::update() {
}
