#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventManager.hpp"
#include "SystemManager.hpp"
#include "Types.hpp"

#include <memory>

namespace Rte {

    class Coordinator {
        public:
            void init() {
                m_componentManager = std::make_unique<ComponentManager>();
                m_entityManager = std::make_unique<EntityManager>();
                m_eventManager = std::make_unique<EventManager>();
                m_systemManager = std::make_unique<SystemManager>();
            }


            // Entity methods
            Entity createEntity() {
                return m_entityManager->createEntity();
            }

            void destroyEntity(Entity entity) {
                m_entityManager->destroyEntity(entity);
                m_componentManager->destroyEntity(entity);
                m_systemManager->destroyEntity(entity);
            }


            // Component methods
            template<typename T>
            void registerComponent() {
                m_componentManager->registerComponent<T>();
            }

            template<typename T>
            void addComponent(Entity entity, T component) {
                m_componentManager->addComponent<T>(entity, component);

                auto signature = m_entityManager->getSignature(entity);
                signature.set(m_componentManager->getComponentType<T>(), true);
                m_entityManager->setSignature(entity, signature);

                m_systemManager->setEntitySignature(entity, signature);
            }

            template<typename T>
            void removeComponent(Entity entity) {
                m_componentManager->removeComponent<T>(entity);

                auto signature = m_entityManager->getSignature(entity);
                signature.set(m_componentManager->getComponentType<T>(), false);
                m_entityManager->setSignature(entity, signature);

                m_systemManager->setEntitySignature(entity, signature);
            }

            template<typename T>
            T& getComponent(Entity entity) {
                return m_componentManager->getComponent<T>(entity);
            }

            template<typename T>
            ComponentType getComponentType() {
                return m_componentManager->getComponentType<T>();
            }


            // System methods
            template<typename T>
            std::shared_ptr<T> registerSystem() {
                return m_systemManager->registerSystem<T>();
            }

            template<typename T>
            void setSystemSignature(Signature signature) {
                m_systemManager->setSignature<T>(signature);
            }


            // Event methods
            void addEventListener(EventType eventType, std::function<void(Event&)> const& listener) {
                m_eventManager->addListener(eventType, listener);
            }

            void sendEvent(Event& event) {
                m_eventManager->sendEvent(event);
            }

            void sendEvent(EventType eventType) {
                m_eventManager->sendEvent(eventType);
            }


        private:
            std::unique_ptr<ComponentManager> m_componentManager;
            std::unique_ptr<EntityManager> m_entityManager;
            std::unique_ptr<EventManager> m_eventManager;
            std::unique_ptr<SystemManager> m_systemManager;
    };

}   // namespace Rte
