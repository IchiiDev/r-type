/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkQueue
*/

#pragma once

#include <cstddef>
#include <deque>
#include <mutex>
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

                bool empty() {
                    std::scoped_lock lock(muxQueue);
                    return deqQueue.empty();
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
    } // namespace net
} // namespace bnl
