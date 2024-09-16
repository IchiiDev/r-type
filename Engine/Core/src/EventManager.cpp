#include "Rte/Ecs/Private/EventManager.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"

#include <functional>
#include <unordered_map>

using namespace Rte;

void EventManager::addListener(EventType eventType, std::function<void(Event&)> const& listener) {
    listeners[eventType].push_back(listener);
}

void EventManager::sendEvent(Event& event) {
    const EventType type = event.getType();
    for (const std::function<void (Rte::Event &)>& listener : listeners[type])
        listener(event);
}

void EventManager::sendEvent(EventType eventType) {
    Event event(eventType);
    for (const auto& listener : listeners[eventType])
        listener(event);
}
