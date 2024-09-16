#pragma once

#include "Rte/Common.hpp"

#include <bitset>
#include <cstdlib>

namespace Rte {

    // ECS
    using Entity = u32;
    const Entity MAX_ENTITIES = 5000;

    using ComponentType = u8;
    const ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;


    // Source: https://gist.github.com/Lee-R/3839813
    constexpr u32 fnv1a_32(const char *s, size_t count) {
        return ((static_cast<bool>(count) ? fnv1a_32(s, count - 1) : 2166136261U) ^ s[count]) * 16777619U;
    }

    constexpr u32 operator "" _hash(char const* s, size_t count) {
        return fnv1a_32(s, count);
    }


    // Events
    using EventType = u32;
    using ParamId = u32;

    #define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), this, std::placeholders::_1)
    #define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&(Listener), std::placeholders::_1)
    #define LAMBDA_LISTENER(EventType, Listener) EventType, Listener

}   // namespace Rte
