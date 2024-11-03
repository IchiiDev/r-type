/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServer
*/

#pragma once

#include "NetworkQueue.hpp"
#include "NetworkConnection.hpp"
#include "NetworkMessage.hpp"
#include "asio/io_context.hpp"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <ostream>
#include <sys/types.h>
#include <system_error>
#include <thread>
#include <utility>

namespace bnl {
    namespace net {
        template<typename T>
        class IServer {
            public:
                IServer(const unsigned int port) 
                    : m_socket(m_asioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)) {
                    }

                virtual ~IServer() {
                    stop();
                }

                bool start() {
                    try {
                        waitForConnection();

                        m_threadContext = std::thread([this]() { m_asioContext.run(); });
                    } catch (const std::exception& e) {
                        std::cerr << "Exception occurred during server start: " << e.what() << std::endl;
                        return false;
                    }

                    std::cout << "[SERVER]: Started!" << std::endl;
                    return true;
                }

                void stop() {
                    m_asioContext.stop();

                    if (m_threadContext.joinable()) m_threadContext.join();

                    std::cout << "[SERVER]: Stopped!" << std::endl;
                }

                void waitForConnection() {
                    m_socket.async_receive_from(
                        asio::buffer(&m_tempHeader, sizeof(message_header<T>)),
                        m_remoteEndpoint,
                        [this](std::error_code ec, std::size_t length) {
                            if (int(m_tempHeader.id) != -1) {
                                waitForConnection();
                                return;
                            }

                            if (!ec) {
                                std::shared_ptr<Connection<T>> newConnection = std::make_shared<Connection<T>>(
                                    Connection<T>::owner::server, 
                                    m_asioContext, 
                                    m_remoteEndpoint, 
                                    m_receivedMessage
                                );

                                newConnection->setId(m_idCounter++);

                                if (onClientConnect(newConnection)) {
                                    m_connectionsQueue.push_back(newConnection);
                                    std::cout << "[SERVER]: Connection approved with client at: " << m_remoteEndpoint << std::endl;

                                    // Start reading from the new connection
                                    newConnection->connectToClient(); // SUUUUUUUUUUUUUUUUS
                                } else {
                                    std::cout << "[SERVER]: Connection denied" << std::endl;
                                }
                            } else {
                                std::cout << "[SERVER]: Connection error: " << ec.message() << std::endl;
                            }

                            // Continue waiting for the next client connection
                            waitForConnection();
                        }
                    );
                }

                void messageClient(std::shared_ptr<Connection<T>> client, const message<T>& msg) {
                    if (client && client->isConnected()) {
                        client->send(msg);
                    } else {
                        onClientDisconnect(client);
                        client.reset();
                        m_connectionsQueue.erase(std::remove(m_connectionsQueue.begin(), m_connectionsQueue.end(), client), m_connectionsQueue.end());
                    }
                }

                void messageAllClient(const message<T>& msg, std::shared_ptr<Connection<T>> ignoredClient = nullptr) {
                    bool invalidClient = false;

                    for (auto& connection : m_connectionsQueue) {
                        if (connection && connection->isConnected()) {
                            if (connection != ignoredClient)
                                connection->send(msg);
                        } else {
                            onClientDisconnect(connection);
                            connection.reset();
                            invalidClient = true;
                        }
                    }

                    if (invalidClient)
                        m_connectionsQueue.erase(std::remove(m_connectionsQueue.begin(), m_connectionsQueue.end(), nullptr), m_connectionsQueue.end());
                }

                void update(size_t maxMsgNbr = SIZE_MAX) {
                    size_t msgNbrCount = 0;
                    while (msgNbrCount < maxMsgNbr && !m_receivedMessage.empty()) {
                        auto msg = m_receivedMessage.popFront();

                        onMessageReceived(msg.remote, msg.msg);

                        msgNbrCount++;
                    }
                }

            protected:
                virtual bool onClientConnect(std::shared_ptr<Connection<T>> client) {
                    return false;
                }

                virtual void onClientDisconnect(std::shared_ptr<Connection<T>> client) {

                }

                virtual void onMessageReceived(std::shared_ptr<Connection<T>> client, message<T>& msg) {

                }

            protected:
                TSQueue<OwnedMessage<T>> m_receivedMessage;

                std::deque<std::shared_ptr<Connection<T>>> m_connectionsQueue;

                asio::io_context m_asioContext;
                std::thread m_threadContext;

                asio::ip::udp::socket m_socket;
                asio::ip::udp::endpoint m_remoteEndpoint;

                uint32_t m_idCounter = 10000;

                message_header<T> m_tempHeader;  // Temporary storage for received message header
        };
    } // namespace net
} // namespace bnl
