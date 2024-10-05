/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkQueue
*/

#pragma once

#include "BetterNetworkLibrary.hpp"
#include "BetterNetworkLibrary/NetworkMessage.hpp"
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <ostream>
#include <utility>

namespace bnl {
    namespace net {
        template <typename T>
        class TSQueue {
            public:
                TSQueue() = default;
                TSQueue(const TSQueue<T>&) = delete;
                virtual ~TSQueue() { clear(); } 
            
            public:
                const T& front() {
                    std::scoped_lock lock(muxQueue);
                    return deqQueue.front();
                }

                const T& back() {
                    std::scoped_lock lock(muxQueue);
                    return deqQueue.back();
                }

                void pushBack(const T& data) {
                    std::scoped_lock lock(muxQueue);
                    deqQueue.emplace_back(std::move(data));
                }

                void pushFront(const T& data) {
                    std::scoped_lock lock(muxQueue);
                    deqQueue.emplace_front(std::move(data));
                }

                void empty() {
                    std::scoped_lock lock(muxQueue);
                    deqQueue.empty();
                }

                size_t size() {
                    std::scoped_lock lock(muxQueue);
                    return deqQueue.size();
                }

                void clear() {
                    std::scoped_lock lock(muxQueue);
                    deqQueue.clear();
                }

                T popFront() {
                    std::scoped_lock lock(muxQueue);
                    auto t = std::move(deqQueue.front());
                    deqQueue.pop_front();
                    return t;
                }

                T popBack() {
                    std::scoped_lock lock(muxQueue);
                    auto t = std::move(deqQueue.front());
                    deqQueue.pop_back();
                    return t;
                }
            
            protected:
                // variable used to keep the queu thread safe
                std::mutex muxQueue;
                // double ended queue for ease of use and performance
                std::deque<T> deqQueue;
        };

        // predefinition for the moment
        template <typename T>
        class connection;


        template<typename T>
        struct OwnedMessage {
            std::shared_ptr<connection<T>> connection = nullptr;
            message<T> msg;

            friend std::ostream& operator << (std::ostream& os, const OwnedMessage<T>&msg) {
                os << msg.msg;
                return os;
            }
        };

    } // namespace net
} // namespace bnl
