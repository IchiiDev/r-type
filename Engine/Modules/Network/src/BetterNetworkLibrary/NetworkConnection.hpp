/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkConnection
*/

#pragma once

#include "NetworkQueue.hpp"
#include "NetworkMessage.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"
#include "asio/post.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <sys/types.h>
#include <system_error>
#include <utility>

namespace bnl {
    namespace net {
        template<typename T>
        class Connection : public std::enable_shared_from_this<Connection<T>> {
            public:
                enum class owner {
                    server,
                    client
                };

            public:
                Connection(owner parent, asio::io_context& ioContext, asio::ip::udp::endpoint endpoint, TSQueue<OwnedMessage<T>>& receiveQueue)
                    : m_asioContext(ioContext), m_socket(ioContext), m_receiveQueue(receiveQueue), m_ownerType(parent), m_remoteEndpoint(std::move(endpoint)) {
                }

                virtual ~Connection() = default;

            public:
                void setId(uint32_t id) { m_id = id; }

                void connectToClient() {
                    if (m_ownerType == owner::server) {
                        m_socket.open(asio::ip::udp::v4());
                        readHeader();
                    }
                }

                void connectToServer(const asio::ip::udp::endpoint& endpoint) {
                    if (m_ownerType == owner::client) {
                        m_remoteEndpoint = endpoint;
                        m_socket.open(asio::ip::udp::v4());
                        readHeader();
                    }
                }

                void disconnect() {
                    if (isConnected())
                        asio::post(m_asioContext, [this]() { m_socket.close(); });
                }

                [[nodiscard]] bool isConnected() const { return m_socket.is_open(); }

            public:
                void send(const message<T>& msg) {
                    asio::post(m_asioContext,
                    [this, msg]() {
                        bool messageBeingWritten = !m_sendQueue.empty();
                        m_sendQueue.pushBack(msg);
                        if (!messageBeingWritten) writeHeader();
                    });
                }

            public:
                [[nodiscard]] uint32_t getId() const { return m_id; }

            private:
                void readHeader() {
                    m_socket.async_receive_from(
                        asio::buffer(&m_tempReceiveMsg.header, sizeof(message_header<T>)),
                        m_remoteEndpoint,
                        [this](std::error_code ec, std::size_t len) {
                            if (!ec) {
                                if (m_tempReceiveMsg.header.size > 0) {
                                    m_tempReceiveMsg.body.resize(m_tempReceiveMsg.header.size);
                                    readBody();
                                } else {
                                    addToIncomingMessageQueue();
                                }
                            } else {
                                std::cerr << "[Connection]: Read header error: " << ec.message() << std::endl;
                            }
                        }
                    );
                }

                void readBody() {
                    m_socket.async_receive_from(
                        asio::buffer(m_tempReceiveMsg.body.data(), m_tempReceiveMsg.body.size()),
                        m_remoteEndpoint,
                        [this](std::error_code ec, std::size_t len) {
                            if (!ec) {
                                addToIncomingMessageQueue();
                            } else {
                                std::cerr << "[Connection]: Read body error: " << ec.message() << std::endl;
                            }
                        }
                    );
                }

                void writeHeader() {
                    m_socket.async_send_to(
                        asio::buffer(&m_sendQueue.front(), sizeof(message_header<T>) + m_sendQueue.front().body.size()),
                        m_remoteEndpoint,
                        [this](std::error_code ec, std::size_t len) {
                            if (!ec) {
                                if (m_sendQueue.front().body.size() > 0) {
                                    writeBody();
                                } else {
                                    m_sendQueue.popFront();
                                    if (!m_sendQueue.empty()) {
                                        writeHeader();
                                    }
                                }
                            } else {
                                std::cerr << "[Connection]: Write header error: " << ec.message() << std::endl;
                            }
                        }
                    );
                }

                void writeBody() {
                    if (!m_socket.is_open()) {
                        std::cerr << "[Connection]: Socket is closed, cannot write" << std::endl;
                        return;
                    }

                    m_socket.async_send_to(
                        asio::buffer(m_sendQueue.front().body.data(), m_sendQueue.front().body.size()),
                        m_remoteEndpoint,
                        [this](std::error_code ec, std::size_t len) {
                            if (!ec) {
                                m_sendQueue.popFront();
                                if (!m_sendQueue.empty()) writeHeader();
                            } else {
                                std::cout << "[" << m_id << "] Write body failed. " << ec.message() << std::endl;
                                std::cout << "message size: " << m_sendQueue.front().body.size() << " > 65,507" << std::endl;

                            }
                        }
                    );
                }

                void addToIncomingMessageQueue() {
                    if (m_ownerType == owner::server)
                        m_receiveQueue.pushBack({ this->shared_from_this(), m_tempReceiveMsg });
                    else
                        m_receiveQueue.pushBack({ nullptr, m_tempReceiveMsg });
                    readHeader();
                }

            protected:
                asio::ip::udp::socket m_socket;
                asio::io_context& m_asioContext;
                asio::ip::udp::endpoint m_remoteEndpoint;

                TSQueue<message<T>> m_sendQueue;
                TSQueue<OwnedMessage<T>>& m_receiveQueue;

                message<T> m_tempReceiveMsg;
                owner m_ownerType = owner::server;
                uint32_t m_id = 0;
        };
    } // namespace net
} // namespace bnl
