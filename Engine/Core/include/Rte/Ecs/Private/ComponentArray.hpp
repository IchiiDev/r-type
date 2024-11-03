#pragma once

#include "Rte/Ecs/Types.hpp"

#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

namespace Rte {

    /**
     * @brief Interface for component arrays to inherit from.
     * This is used to allow for a vector of pointers of different templated types.
     */
    class IComponentArray { // NOLINT(cppcoreguidelines-special-member-functions)
        public:
            virtual ~IComponentArray() = default;

            /**
             * @brief Remove the entity data from the array.
             *
             * @param entity The entity that was destroyed.
             */
            virtual void destroyEntity(Entity entity) = 0;
    };


    /**
     * @brief Templated class for storing component data in contiguous memory.
     *
     * @tparam T The type of component to store.
     */
    template<typename T>
    class ComponentArray : public IComponentArray {
        public:
            ComponentArray() = default;
            ~ComponentArray() override = default;

            ComponentArray(const ComponentArray&) = delete;
            ComponentArray& operator=(const ComponentArray&) = delete;

            ComponentArray(ComponentArray&&) noexcept = default;
            ComponentArray& operator=(ComponentArray&&) noexcept = default;


            /**
             * @brief Insert the datas of an entity into the array.
             * /!\ The entity must not already have a component of this type.
             *
             * @param entity The entity to insert the component data of.
             * @param component The component data to insert.
             */
            void insertComponent(Entity entity, T component) {
                assert(m_entityToComponentIndexMap.find(entity) == m_entityToComponentIndexMap.end() && "Cannot insert component into the array: entity already has this component.");

                // Add the component to the end of the array
                const size_t newIndex = m_size;
                m_componentArray[newIndex] = component;

                // Update the maps to point to the new component
                m_entityToComponentIndexMap[entity] = newIndex;
                m_componentIndexToEntityMap[newIndex] = entity;

                ++m_size;
            }

            /**
             * @brief Remove the component data of an entity from the array.
             * /!\ The entity must have a component of this type.
             *
             * @param entity The entity to remove the component data of.
             */
            void removeComponent(Entity entity) {
                assert(m_entityToComponentIndexMap.find(entity) != m_entityToComponentIndexMap.end() && "Cannot remove component from the array: no such component found.");

                // Copy the last component data to the position of the deleted one
                const size_t lastComponentIndex = m_size - 1;
                const size_t targetComponentIndex = m_entityToComponentIndexMap[entity];
                m_componentArray[targetComponentIndex] = m_componentArray[lastComponentIndex];

                // Update the maps to point to the new component
                const Entity entityOfLastComponent = m_componentIndexToEntityMap[lastComponentIndex];
                m_entityToComponentIndexMap[entityOfLastComponent] = targetComponentIndex;
                m_componentIndexToEntityMap[targetComponentIndex] = entityOfLastComponent;

                // Remove the deleted entity's component data from the maps
                m_entityToComponentIndexMap.erase(entity);
                m_componentIndexToEntityMap.erase(lastComponentIndex);

                --m_size;
            }

            /**
             * @brief Get the component data of an entity.
             * /!\ The entity must have a component of this type.
             *
             * @param entity The entity to get the component data of.
             * @return T& The component data.
             */
            T& getComponent(Entity entity) {
                assert(m_entityToComponentIndexMap.find(entity) != m_entityToComponentIndexMap.end() && "Cannot retrieve component from the array: no such component found.");
                return m_componentArray[m_entityToComponentIndexMap[entity]];
            }

            /**
             * @brief Remove the entity data from the array.
             *
             * @param entity The entity that was destroyed.
             */
            void destroyEntity(Entity entity) override {
                if (m_entityToComponentIndexMap.find(entity) != m_entityToComponentIndexMap.end())
                    removeComponent(entity);
            }

        private:
            std::array<T, MAX_ENTITIES> m_componentArray;
            std::unordered_map<Entity, size_t> m_entityToComponentIndexMap;
            std::unordered_map<size_t, Entity> m_componentIndexToEntityMap;
            size_t m_size{};
    };

}   // namespace Rte
