#include "PhysicsSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/Components.hpp"
#include "RigidBodyImpl.hpp"
#include "Rte/Physics/Tool.hpp"

#include "box2d/box2d.h"
#include "box2d/math_functions.h"

#include <cassert>
#include <iostream>
#include <memory>

using namespace Rte::Physics;

void PhysicsSystem::init(const std::shared_ptr<Rte::Ecs>& ecs, b2WorldId worldId, float timeStep, int subStepCount) {
    m_ecs = ecs;
    m_worldId = worldId;
    m_timeStep = timeStep;
    m_subStepCount = subStepCount;
}

void PhysicsSystem::update() {
    assert(m_ecs != nullptr && "Cannot update render system: Not initialized.");

    b2World_Step(m_worldId, m_timeStep, m_subStepCount);
    for (const Entity entity : m_entities) {
        const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
        BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
        b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
        b2Rot rotation = b2Body_GetRotation(rigidBody->getBodyId());
        

        transformComponent.position = {(position.x * 8.F * PPM + 1920 / 2.F), -(position.y * 8.F * PPM - 1080 / 2.F)};
        transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / b2_pi;
    }
}
