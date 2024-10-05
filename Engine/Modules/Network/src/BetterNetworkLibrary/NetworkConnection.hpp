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
#include <memory>

namespace bnl {
    namespace net {
        template<typename T>
        class Connection : public std::enable_shared_from_this<Connection<T>> {
            public:
                Connection() {}
                virtual ~Connection() {}
            
            public:
                bool connectedToServer();
                bool disconnect();
                bool isConnected() const;
            
            public:
                bool send(const message<T>& msg);
            
            protected:
                asio::ip::tcp::socket m_socket;
                asio::io_context& m_asioContext;

                TSQueue<message<T>> m_sendQueue;
                // SUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUSSSSSSSSSSSSSSSSSSSS
                TSQueue<OwnedMessage<T>>& m_receiveQueue;
        };
    } // namespace net
} // namespace bnl