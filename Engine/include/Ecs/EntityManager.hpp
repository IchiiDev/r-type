#pragma once

#include "Types.hpp"

#include <array>
#include <cassert>
#include <queue>

namespace Rte {

    /**
     * @brief The entity manager class is responsible for managing the entities.
     * It is responsible for creating and destroying entities.
     * It also provides a way to set and get the signature of an entity.
     */
    class EntityManager {
        public:
            EntityManager() {
                // Initialize the queue with all possible entity IDs.
                for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
                    m_availableEntities.push(entity);
            }

            /**
             * @brief Create a new entity by getting the first available entity ID from the queue.
             * /!\ The living entity count must be less than the maximum number of entities.
             *
             * @return Entity The new entity.
             */
            Entity createEntity() {
                assert(m_livingEntityCount < MAX_ENTITIES && "Cannot create entity: too many entities alive.");

                const Entity newEntity = m_availableEntities.front();
                m_availableEntities.pop();
                ++m_livingEntityCount;

                return newEntity;
            }

            /**
             * @brief Destroy an entity by putting it back in the available entities queue.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to destroy.
             */
            void destroyEntity(Entity entity) {
                assert(entity < MAX_ENTITIES && "Cannot destroy entity: id out of range.");

                m_signatures[entity].reset();
                m_availableEntities.push(entity);
                --m_livingEntityCount;
            }

            /**
             * @brief Set the signature of an entity.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to set the signature of.
             * @param signature The signature to set.
             */
            void setSignature(Entity entity, Signature signature) {
                assert(entity < MAX_ENTITIES && "Cannot set entity signature: id out of range.");
                m_signatures[entity] = signature;
            }

            /**
             * @brief Get the signature of an entity.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to get the signature of.
             * @return Signature The signature of the entity.
             */
            Signature getSignature(Entity entity) {
                assert(entity < MAX_ENTITIES && "Cannot get entity signature: id out of range.");
                return m_signatures[entity];
            }

        private:
            std::queue<Entity> m_availableEntities;
            std::array<Signature, MAX_ENTITIES> m_signatures;
            uint32_t m_livingEntityCount{};
    };

}   // namespace Rte
