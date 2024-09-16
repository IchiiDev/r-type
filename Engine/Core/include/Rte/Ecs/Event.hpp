#pragma once

#include "Types.hpp"

#include <any>
#include <unordered_map>

namespace Rte {

    /**
     * @brief The event class is a container for an event and its parameters.
     * It is used to send events to the event manager.
     */
    class Event {
        public:
            Event() = delete;
            explicit Event(EventType type) : m_type(type) {}

            /**
             * @brief Set the parameter of an event.
             *
             * @tparam T The type of the parameter to set.
             * @param id The id of the parameter to set.
             * @param value The value of the parameter to set.
             */
            template<typename T>
            void setParameter(ParamId id, T value) {
                m_data[id] = value;
            }

            /**
             * @brief Get the parameter of an event.
             *
             * @tparam T The type of the parameter to get.
             * @param id The id of the parameter to get.
             * @return T The value of the parameter.
             */
            template<typename T>
            [[nodiscard]] T getParameter(ParamId id) {
                return std::any_cast<T>(m_data[id]);
            }

            /**
             * @brief Get the type of the event.
             *
             * @return ParamId The type of the event.
             */
            [[nodiscard]] ParamId getType() const {
                return m_type;
            }

        private:
            EventType m_type{};
            std::unordered_map<ParamId, std::any> m_data;
    };

}   // namespace Rte
