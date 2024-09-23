#pragma once

#include "Rte/Common.hpp"

#include <bitset>
#include <cstdlib>
#include <set>

namespace Rte {

    // ECS
    using Entity = u32;
    const Entity MAX_ENTITIES = 5000;

    using ComponentType = u8;
    const ComponentType MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

    /**
     * @brief A system is a collection of entities that share the same components.
     * After registration, entities possessed by the system are updated automatically by the ECS at each entity or component update.
     * /!\ All systems must inherit from this class.
     */
    class System {
        public:
            System() = default;
            virtual ~System() = default;

            System(const System&) = delete;
            System& operator=(const System&) = delete;
            System(System&&) noexcept = default;
            System& operator=(System&&) noexcept= default;

        public:
            /**
             * @brief Entities possessed by the system.
             * They will be updated automatically by the ECS.
             */
            std::set<Entity> m_entities;
    };



    // Hash fonctions for events (Source: https://gist.github.com/Lee-R/3839813)
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
