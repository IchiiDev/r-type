/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClient
*/

#pragma once

#include "BetterNetworkLibrary/NetworkConnection.hpp"
#include "NetworkQueue.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>

namespace bnl {
    namespace net {
        template <typename T>
        class IClient {
            public:
                IClient() : m_socket(m_io_context) {};
                virtual ~IClient() { disconnect(); }

            public:
                bool connect(const std::string& host, const unsigned int port) {
                    try {
                        m_connection = std::make_unique<Connection<T>>(); // TODO: implement the connection object

                        asio::ip::tcp::resolver resolver(m_io_context);
                        m_endpoints = resolver.resolve(host, std::to_string(port));

                        m_connection->connectToServer(m_endpoints);

                        thrContext = std::thread([this]() { m_io_context.run(); });
                    } catch (const std::exception& e) {
                        std::cerr << "Exeption occured during client connection" << e.what() << std::endl;
                        return false;
                    }

                    return true;
                }

                void disconnect() {
                    if (m_connection)
                        m_connection->disconnect();

                    m_io_context.stop();

                    if (thrContext.joinable())
                        thrContext.join();

                    m_connection.release();
                }

                bool isConnected() {
                    if (m_connection)
                        return m_connection->isConnected();

                    return false;
                }

                TSQueue<OwnedMessage<T>> &getReiceiveQueue() {
                    return m_receiveQueue;
                }

            protected:
                asio::io_context m_io_context;
                std::thread thrContext;
                asio::ip::tcp::socket m_socket;
                std::unique_ptr<Connection<T>> m_connection;

            private:
                TSQueue<OwnedMessage<T>> m_receiveQueue;
        };
    } // namespace net
} // namespace bnl