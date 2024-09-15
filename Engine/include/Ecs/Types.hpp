#pragma once

#include <bitset>
#include <cstdint>

namespace Rte {

    // ECS
    using Entity = std::uint32_t;
    const Entity MAX_ENTITIES = 5000;

    using ComponentType = std::uint8_t;
    const ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;


    // Source: https://gist.github.com/Lee-R/3839813
    constexpr std::uint32_t fnv1a_32(const char *s, std::size_t count) {
        return ((static_cast<bool>(count) ? fnv1a_32(s, count - 1) : 2166136261U) ^ s[count]) * 16777619U;
    }

    constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count) {
        return fnv1a_32(s, count);
    }


    // Events
    using EventType = std::uint32_t;
    using ParamId = std::uint32_t;

    #define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), this, std::placeholders::_1)
    #define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), std::placeholders::_1)
    #define LAMBDA_LISTENER(EventType, Listener) EventType, Listener

}   // namespace Rte
