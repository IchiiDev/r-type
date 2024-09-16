#include "Rte/Ecs/EntityManager.hpp"
#include "Rte/Ecs/Types.hpp"

#include <cassert>

using namespace Rte;

EntityManager::EntityManager() {
    // Initialize the queue with all possible entity IDs.
    for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        m_availableEntities.push(entity);
}

Entity EntityManager::createEntity() {
    assert(m_livingEntityCount < MAX_ENTITIES && "Cannot create entity: too many entities alive.");

    const Entity newEntity = m_availableEntities.front();
    m_availableEntities.pop();
    ++m_livingEntityCount;

    return newEntity;
}

void EntityManager::destroyEntity(Entity entity) {
    assert(entity < MAX_ENTITIES && "Cannot destroy entity: id out of range.");

    m_signatures[entity].reset();
    m_availableEntities.push(entity);
    --m_livingEntityCount;
}

void EntityManager::setSignature(Entity entity, Signature signature) {
    assert(entity < MAX_ENTITIES && "Cannot set entity signature: id out of range.");
    m_signatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity) {
    assert(entity < MAX_ENTITIES && "Cannot get entity signature: id out of range.");
    return m_signatures[entity];
}
