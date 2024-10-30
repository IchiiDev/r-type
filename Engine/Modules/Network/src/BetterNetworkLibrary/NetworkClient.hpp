/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClient
*/

#pragma once

#include "NetworkConnection.hpp"
#include "NetworkMessage.hpp"
#include "NetworkQueue.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include <iostream>
#include <memory>
#include <ostream>
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
                        asio::ip::udp::resolver resolver(m_io_context);
                        asio::ip::udp::endpoint endpoint = *resolver.resolve(host, std::to_string(port));

                        m_connection = std::make_unique<Connection<T>>(
                            Connection<T>::owner::client,
                            m_io_context,
                            endpoint, // Here make sure that pass out the correct thing
                            m_receiveQueue
                        );

                        std::cout << host << ":" << port << std::endl;
                        m_connection->connectToServer(endpoint);

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

            public:
                TSQueue<OwnedMessage<T>> &getReiceiveQueue() {
                    return m_receiveQueue;
                }

                void send(const message<T>& msg) {
                    if (isConnected()) {
                        m_connection->send(msg);
                    }
			    }

            protected:
                asio::io_context m_io_context;
                std::thread thrContext;
                asio::ip::udp::socket m_socket;
                std::unique_ptr<Connection<T>> m_connection;

            private:
                TSQueue<OwnedMessage<T>> m_receiveQueue;
        };
    } // namespace net
} // namespace bnl