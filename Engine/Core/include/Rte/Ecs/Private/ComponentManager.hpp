#pragma once

#include "Rte/Ecs/Private/ComponentArray.hpp"
#include "Rte/Ecs/Types.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

namespace Rte {

    /**
     * @brief The component manager class is responsible for managing the component arrays.
     * It is responsible for registering components, adding components to entities, and removing components from entities.
     * It also provides a way to get the component data of an entity.
     */
    class ComponentManager {
        public:
            /**
             * @brief Register a component type.
             * /!\ Each component type must be unique. (No two components of the same type can be registered.)
             *
             * @tparam T The type of component to register.
             */
            template<typename T>
            void registerComponent() {
                const std::string typeName = typeid(T).name();
                assert(m_componentTypes.find(typeName) == m_componentTypes.end() && "Cannot register component: already registered.");

                m_componentTypes.insert({typeName, m_nextComponentType});
                m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

                ++m_nextComponentType;
            }

            /**
             * @brief Get the type of a component.
             * /!\ The component must be registered before use.
             *
             * @tparam T The type of component to get the type of.
             * @return ComponentType The type of the component.
             */
            template<typename T>
            ComponentType getComponentType() {
                const std::string typeName = typeid(T).name();
                assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Cannot get component type: component not registered before use.");
                return m_componentTypes[typeName];
            }

            /**
             * @brief Add a component to an entity.
             * /!\ The component must be registered before use.
             * /!\ The entity must not already have a component of this type.
             *
             * @tparam T The type of component to add.
             * @param entity The entity to add the component to.
             * @param component The component data to add.
             */
            template<typename T>
            void addComponent(Entity entity, T component) {
                assert(m_componentTypes.find(typeid(T).name()) != m_componentTypes.end() && "Cannot add component: component not registered before use.");
                getComponentArray<T>()->insertComponent(entity, component);
            }

            /**
             * @brief Remove a component from an entity.
             * /!\ The component must be registered before use.
             * /!\ The entity must have a component of this type.
             *
             * @tparam T The type of component to remove.
             * @param entity The entity to remove the component from.
             */
            template<typename T>
            void removeComponent(Entity entity) {
                assert(m_componentTypes.find(typeid(T).name()) != m_componentTypes.end() && "Cannot remove component: component not registered before use.");
                getComponentArray<T>()->removeComponent(entity);
            }

            /**
             * @brief Get the component data of an entity.
             * /!\ The component must be registered before use.
             * /!\ The entity must have a component of this type.
             *
             * @tparam T The type of component to get.
             * @param entity The entity to get the component data of.
             * @return T& The component data.
             */
            template<typename T>
            T& getComponent(Entity entity) {
                assert(m_componentTypes.find(typeid(T).name()) != m_componentTypes.end() && "Cannot get component: component not registered before use.");
                return getComponentArray<T>()->getComponent(entity);
            }

            /**
             * @brief Remove all components of an entity.
             *
             * @param entity The entity to remove all components of.
             */
            void destroyEntity(Entity entity) {
                for (const auto& componentArray : m_componentArrays)
                    componentArray.second->destroyEntity(entity);
            }

        private:
            /**
             * @brief Get the component array of a component type.
             * /!\ The component must be registered before use.
             *
             * @tparam T The type of component to get the array of.
             * @return std::shared_ptr<ComponentArray<T>> The component array.
             */
            template<typename T>
            std::shared_ptr<ComponentArray<T>> getComponentArray() {
                const std::string typeName = typeid(T).name();
                assert(m_componentTypes.find(typeName) != m_componentTypes.end() && "Cannot get component array: no component array of this type found.");
                return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
            }

        private:
            std::unordered_map<std::string, ComponentType> m_componentTypes;
            std::unordered_map<std::string, std::shared_ptr<IComponentArray>> m_componentArrays;
            ComponentType m_nextComponentType{};

    };

}   // namespace Rte
