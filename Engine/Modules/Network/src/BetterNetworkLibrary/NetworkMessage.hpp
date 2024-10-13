/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkMessage
*/

#pragma once

#include "Rte/Common.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <sys/types.h>
#include <type_traits>
#include <vector>

namespace bnl {
    namespace net {

        template <typename T>
        struct message_header {
            T id{};
            uint32_t size = 0;
        };

        template <typename T>
        struct message {
            message_header<T> header;
            std::vector<uint8_t> body;

            [[nodiscard]] size_t size() const { return body.size(); }

            // debug function
            friend std::ostream& operator << (std::ostream& os, const message<T>& msg) {
                os << "Message ID: " << static_cast<int>(msg.header.id) << ", Size: " << msg.header.size;
                return os;
            }

            /* Can allow some stuff like that:

                message<int> message;
                message << 123;
                message << "Hello, World!";
                message << -0.65;
            */
            template <typename DataType>
            friend message<T>& operator << (message<T>& msg, const DataType &data) {
                // template only supports standard layout
                static_assert(std::is_standard_layout<DataType>::value, "DataType must be a standard layout");

                size_t s = msg.body.size();
                msg.body.resize(s + sizeof(DataType));
                std::memcpy(msg.body.data() + s, &data, sizeof(DataType));

                msg.header.size = msg.size();

                return msg;
            }

            template <typename DataType>
            friend message<T>& operator >> (message<T>& msg, DataType &data) {
                static_assert(std::is_standard_layout<DataType>::value, "DataType must be a standard layout");

                size_t s = msg.body.size() - sizeof(DataType);
                std::memcpy(&data, msg.body.data() + s, sizeof(DataType));
                msg.body.resize(s);

                msg.header.size = msg.size();

                return msg;
            }
        };

        // predefinition for the moment
        template <typename T>
        class Connection;


        template<typename T>
        struct OwnedMessage {
            std::shared_ptr<Connection<T>> remote = nullptr;
            message<T> msg;

            friend std::ostream& operator << (std::ostream& os, const OwnedMessage<T>&msg) {
                os << msg.msg;
                return os;
            }
        };

    } // namespace net
} // namespace bnl