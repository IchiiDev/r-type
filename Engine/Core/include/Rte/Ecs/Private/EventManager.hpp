#pragma once

#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"

#include <functional>
#include <list>
#include <unordered_map>

namespace Rte {

    /**
     * @brief EventManager class is responsible for managing events and listeners.
     * It allows to add listeners to specific event types and send events to all listeners.
     */
    class EventManager {
        public:
            EventManager() = default;
            ~EventManager() = default;

            EventManager(const EventManager&) = delete;
            EventManager& operator=(const EventManager&) = delete;

            EventManager(EventManager&&) noexcept = default;
            EventManager& operator=(EventManager&&) noexcept = default;


            /**
             * @brief Add a listener to a specific event type.
             *
             * @param eventType The type of event to listen to.
             * @param listener The listener function to be called when the event is sent.
             */
            void addListener(EventType eventType, std::function<void(Event&)> const& listener);

            /**
             * @brief Send an event to all listeners of the event type.
             *
             * @param event The event to send.
             */
            void sendEvent(Event& event);

            /**
             * @brief Send an event to all listeners of the event type.
             *
             * @param eventType The type of event to send.
             */
            void sendEvent(EventType eventType);

        private:
            std::unordered_map<EventType, std::list<std::function<void(Event&)>>> listeners;
    };

}   // namespace Rte
