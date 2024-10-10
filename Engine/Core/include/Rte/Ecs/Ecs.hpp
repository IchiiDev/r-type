#pragma once

#include "Rte/BasicComponents.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Private/ComponentManager.hpp"
#include "Rte/Ecs/Private/EntityManager.hpp"
#include "Rte/Ecs/Private/EventManager.hpp"
#include "Rte/Ecs/Private/SystemManager.hpp"
#include "Rte/Ecs/Types.hpp"

#include <cassert>
#include <functional>
#include <memory>

namespace Rte {

    class Ecs {
        public:
            Ecs() {
                m_componentManager = std::make_unique<ComponentManager>();
                m_entityManager = std::make_unique<EntityManager>();
                m_eventManager = std::make_unique<EventManager>();
                m_systemManager = std::make_unique<SystemManager>();

                this->registerComponent<BasicComponents::Transform>();
                this->registerComponent<BasicComponents::UidComponents>();
            }




            //////////////////////////
            ///// ENTITY METHODS /////
            //////////////////////////

            /**
             * @brief Create a new blank entity.
             *
             * @return Entity The entity created.
             */
            Entity createEntity() {
                return m_entityManager->createEntity();
            }

            /**
             * @brief Destroy an entity.
             *
             * @param entity The entity to destroy.
             */
            void destroyEntity(Entity entity) {
                m_entityManager->destroyEntity(entity);
                m_componentManager->destroyEntity(entity);
                m_systemManager->destroyEntity(entity);
            }




            /////////////////////////////
            ///// COMPONENT METHODS /////
            /////////////////////////////

            /**
             * @brief Register a component.
             * /!\ A component must be registered before it can be added to an entity.
             * /!\ A component must be registered before a system can use it.
             *
             * @tparam T The component to register.
             */
            template<typename T>
            void registerComponent() {
                m_componentManager->registerComponent<T>();
            }

            /**
             * @brief Add a component to an entity.
             * /!\ The component must be registered before it can be added to an entity.
             * /!\ The entity must be a valid entity created using createEntity().
             *
             * @tparam T The component to add.
             * @param entity The entity to add the component to.
             * @param component The component to add.
             */
            template<typename T>
            void addComponent(Entity entity, T component) {
                // Component manager update
                m_componentManager->addComponent<T>(entity, component);

                // Entity manager update
                Signature signature = m_entityManager->getSignature(entity);
                signature.set(m_componentManager->getComponentType<T>(), true);
                m_entityManager->setSignature(entity, signature);

                // System manager update
                m_systemManager->setEntitySignature(entity, signature);
            }

            /**
             * @brief Remove a component from an entity.
             * /!\ The entity must have the component to remove it.
             *
             * @tparam T The component to remove.
             * @param entity The entity to remove the component from.
             */
            template<typename T>
            void removeComponent(Entity entity) {
                // Component manager update
                m_componentManager->removeComponent<T>(entity);

                // Entity manager update
                Signature signature = m_entityManager->getSignature(entity);
                signature.set(m_componentManager->getComponentType<T>(), false);
                m_entityManager->setSignature(entity, signature);

                // System manager update
                m_systemManager->setEntitySignature(entity, signature);
            }

            /**
             * @brief Get a component from an entity.
             * /!\ The entity must have the component to get it.
             *
             * @tparam T The component to get.
             * @param entity The entity to get the component from.
             * @return T& The component.
             */
            template<typename T>
            T& getComponent(Entity entity) {
                return m_componentManager->getComponent<T>(entity);
            }

            /**
             * @brief Get the component type of a component.
             * Use this method to get the type of a component to use it in a signature.
             * /!\ The component must be registered before getting its type.
             *
             * @tparam T The component to get the type of.
             * @return ComponentType The type of the component.
             */
            template<typename T>
            ComponentType getComponentType() {
                return m_componentManager->getComponentType<T>();
            }




            //////////////////////////
            ///// SYSTEM METHODS /////
            //////////////////////////

            /**
             * @brief Register a system.
             * /!\ A system must be registered before it can be used and updated.
             * /!\ A system must be registered before its signature can be set.
             *
             * @tparam T The system to register.
             * @return std::shared_ptr<T> The system registered.
             */
            template<typename T>
            std::shared_ptr<T> registerSystem() {
                return m_systemManager->registerSystem<T>();
            }

            /**
             * @brief Set the signature of a system.
             * /!\ The system must be registered before its signature can be set.
             *
             * @tparam T The system to set the signature of.
             * @param signature The signature to set.
             */
            template<typename T>
            void setSystemSignature(Signature signature) {
                m_systemManager->setSignature<T>(signature);
            }




            /////////////////////////
            ///// EVENT METHODS /////
            /////////////////////////

            /**
             * @brief Add an event listener.
             *
             * @param eventType The type of event to listen to.
             * @param listener A listener made using "METHOD_LISTENER", "FUNCTION_LISTENER" or "LAMBDA_LISTENER".
             */
            void addEventListener(EventType eventType, std::function<void(Event&)> const& listener) {
                m_eventManager->addListener(eventType, listener);
            }

            /**
             * @brief Send an event to all listeners that listen to its type.
             *
             * @param event The event to send.
             */
            void sendEvent(Event& event) {
                m_eventManager->sendEvent(event);
            }

            /**
             * @brief Send an event to all listeners that listen to its type.
             *
             * @param eventType The type of event to send.
             */
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
