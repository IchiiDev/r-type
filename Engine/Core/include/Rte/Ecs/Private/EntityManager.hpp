#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"

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
            EntityManager();
            ~EntityManager() = default;

            EntityManager(const EntityManager&) = delete;
            EntityManager& operator=(const EntityManager&) = delete;

            EntityManager(EntityManager&&) noexcept = default;
            EntityManager& operator=(EntityManager&&) noexcept = default;



            /**
             * @brief Create a new entity by getting the first available entity ID from the queue.
             * /!\ The living entity count must be less than the maximum number of entities.
             *
             * @return Entity The new entity.
             */
            Entity createEntity();

            /**
             * @brief Destroy an entity by putting it back in the available entities queue.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to destroy.
             */
            void destroyEntity(Entity entity);

            /**
             * @brief Set the signature of an entity.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to set the signature of.
             * @param signature The signature to set.
             */
            void setSignature(Entity entity, Signature signature);

            /**
             * @brief Get the signature of an entity.
             * /!\ The entity must be alive.
             *
             * @param entity The entity to get the signature of.
             * @return Signature The signature of the entity.
             */
            Signature getSignature(Entity entity);

        private:
            std::queue<Entity> m_availableEntities;
            std::array<Signature, MAX_ENTITIES> m_signatures;
            u32 m_livingEntityCount{};
    };

}   // namespace Rte
