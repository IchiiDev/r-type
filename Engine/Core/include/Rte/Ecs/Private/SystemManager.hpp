#pragma once

#include "Rte/Ecs/Types.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>
#include <utility>

namespace Rte {

    /**
     * @brief The system manager class is responsible for managing the systems.
     * It is responsible for registering systems and setting their signatures.
     * It also provides a way to notify the systems when an entity is destroyed or when its signature changes.
     */
    class SystemManager {
        public:
            /**
             * @brief Register a system of type T.
             * /!\ The system must not be already registered.
             *
             * @tparam T The type of the system to register.
             * @return std::shared_ptr<T> The shared pointer to the registered system.
             */
            template<typename T>
            std::shared_ptr<T> registerSystem() {
                const char* typeName = typeid(T).name();
                assert(m_systems.find(typeName) == m_systems.end() && "Cannot register system: already registered.");

                std::shared_ptr<T> system = std::make_shared<T>();
                m_systems.insert({typeName, system});
                return system;
            }

            /**
             * @brief Set the signature of a system of type T.
             * /!\ The system must be registered.
             *
             * @tparam T The type of the system to set the signature of.
             * @param signature The signature to set.
             */
            template<typename T>
            void setSignature(Signature signature) {
                const char* typeName = typeid(T).name();
                assert(m_systems.find(typeName) != m_systems.end() && "Cannot set system signature: system not registered.");

                m_signatures.insert({typeName, signature});
            }

            /**
             * @brief Remove the entity from all the systems.
             *
             * @param entity The entity that has been destroyed.
             */
            void destroyEntity(Entity entity) {
                for (const std::pair<const char *const, std::shared_ptr<System>>& system : m_systems)
                    system.second->m_entities.erase(entity);
            }

            /**
             * @brief Set the signature of an entity and add or remove it from the systems.
             *
             * @param entity The entity that has changed.
             * @param entitySignature The new signature of the entity.
             */
            void setEntitySignature(Entity entity, Signature entitySignature) {
                for (const std::pair<const char *const, std::shared_ptr<Rte::System>>& system : m_systems) {
                    if ((entitySignature & m_signatures[system.first]) == m_signatures[system.first])
                        system.second->m_entities.insert(entity);
                    else
                        system.second->m_entities.erase(entity);
                }
            }

        private:
            std::unordered_map<const char*, Signature> m_signatures;
            std::unordered_map<const char*, std::shared_ptr<System>> m_systems;
    };

}   // namespace Rte
