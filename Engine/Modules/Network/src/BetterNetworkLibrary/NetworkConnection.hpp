/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkConnection
*/

#pragma once

#include "NetworkCommon.hpp"
#include "NetworkQueue.hpp"
#include "NetworkMessage.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "asio/post.hpp"
#include "asio/read.hpp"
#include "asio/write.hpp"
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
                Connection(owner parent, asio::io_context& ioContext, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<T>>& receiveQueue)
                    : m_asioContext(ioContext), m_socket(std::move(socket)), m_receiveQueue(receiveQueue), m_ownerType(parent) {
                }
                virtual ~Connection() = default;

            public:
                void setId(uint32_t id) { m_id = id; }

                void connectToClient() {
                    if (m_ownerType == owner::server)
                        if (m_socket.is_open())
                            readHeader();
                }

                void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
                    if (m_ownerType == owner::client) {
                        asio::async_connect(m_socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
                            if (!ec) {
                                readHeader();
                            } else {
                                std::cerr << "[Connection]: Connect to server error: " << ec.message() << std::endl;
                                m_socket.close();
                            }
                        });
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
                    asio::async_read(m_socket, asio::buffer(&m_tempReceiveMsg.header, sizeof(message_header<T>)),
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
                                m_socket.close();
                            }
                        }
                    );
                }

                void readBody() {
                    asio::async_read(m_socket, asio::buffer(m_tempReceiveMsg.body.data(), m_tempReceiveMsg.body.size()),
                        [this](std::error_code ec, std::size_t len) {
                            if (!ec) {
                                addToIncomingMessageQueue();
                            } else {
                                std::cerr << "[Connection]: Read body error: " << ec.message() << std::endl;
                                m_socket.close();
                            }
                        }
                    );
                }

                void writeHeader() {
                    asio::async_write(m_socket, asio::buffer(&m_sendQueue.front().header, sizeof(message_header<T>)),
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
                                m_socket.close();
                            }
                        }
                    );
                }

                void writeBody() {
                    asio::async_write(m_socket, asio::buffer(m_sendQueue.front().body.data(), m_sendQueue.front().body.size()),
                        [this](std::error_code ec, std::size_t length) {
                            if (!ec) {
                                m_sendQueue.popFront();

                                if (!m_sendQueue.empty())
                                    writeHeader();
                            } else {
                                std::cout << "[" << m_id << "] Write body failed.\n";
                                m_socket.close();
                            }
                        });
                }

                void addToIncomingMessageQueue() {
                    if (m_ownerType == owner::server)
                        m_receiveQueue.pushBack({ this->shared_from_this(), m_tempReceiveMsg });
                    else
                        m_receiveQueue.pushBack ({ nullptr, m_tempReceiveMsg });

                    readHeader();
                }

            protected:
                asio::ip::tcp::socket m_socket;
                asio::io_context& m_asioContext;

                TSQueue<message<T>> m_sendQueue;
                TSQueue<OwnedMessage<T>>& m_receiveQueue;

                message<T> m_tempReceiveMsg;

                owner m_ownerType = owner::server;
                uint32_t m_id = 0;
        };
    } // namespace net
} // namespace bnl