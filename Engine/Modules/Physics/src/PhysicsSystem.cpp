#include "PhysicsSystem.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/Components.hpp"
#include "RigidBodyImpl.hpp"
#include "PlayerBodyImpl.hpp"
#include "Rte/Physics/Materials.hpp"
#include "SandBoxImpl.hpp"
#include "Rte/Physics/Tool.hpp"

#include "box2d/box2d.h"
#include "box2d/math_functions.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

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

    std::shared_ptr<SandBoxImpl> sandBox;
    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).sandBox) {
            sandBox = interfaceCast<SandBoxImpl>(m_ecs->getComponent<Components::Physics>(entity).sandBox);
        }
    }
    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            if (!rigidBody->isDynamic()) {
                BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
                b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
                b2Rot rotation = b2Body_GetRotation(rigidBody->getBodyId());

                transformComponent.position = {(position.x * 8.F * PPM + 1920 / 2.F), -(position.y * 8.F * PPM - 1080 / 2.F)};
                transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / b2_pi;
                std::vector<std::vector<pixel>> staticBodyPixels = rigidBody->getRotatedPixels();
                for (int i = 0; i < staticBodyPixels.size(); i++) {
                    for (int j = 0; j < staticBodyPixels[i].size(); j++) {
                        if (staticBodyPixels[i][j].r != 0 || staticBodyPixels[i][j].g != 0 || staticBodyPixels[i][j].b != 0)
                        sandBox->changePixel({static_cast<int>(std::round(staticBodyPixels[i][j].pos.x + position.x * PPM + static_cast<float>(sandBox->getSize().x) / 2.F)), static_cast<int>(std::round(staticBodyPixels[i][j].pos.y - position.y * PPM + static_cast<float>(sandBox->getSize().y) / 2.F))}, {s_wood, invMatsColors.at(s_wood), 0});
                    }
                }
            }
        }
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).playerBody) {
            const std::shared_ptr<PlayerBodyImpl>& playerBody = interfaceCast<PlayerBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).playerBody);
            BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
            b2Vec2 position = b2Body_GetPosition(playerBody->getBodyId());
            b2Rot rotation = b2Body_GetRotation(playerBody->getBodyId());

            transformComponent.position = {(position.x * 8.F * PPM + 1920 / 2.F), -(position.y * 8.F * PPM - 1080 / 2.F)};
            transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / b2_pi;
        }
    }
    if (sandBox)
        sandBox->Step();
    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            if (!rigidBody->isDynamic()) {
                b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
                std::vector<std::vector<pixel>> staticBodyPixels = rigidBody->getRotatedPixels();
                for (int i = 0; i < staticBodyPixels.size(); i++) {
                    for (int j = 0; j < staticBodyPixels[i].size(); j++) {
                        if (staticBodyPixels[i][j].r != 0 || staticBodyPixels[i][j].g != 0 || staticBodyPixels[i][j].b != 0)
                        sandBox->changePixel({static_cast<int>(std::round(staticBodyPixels[i][j].pos.x + position.x * PPM + static_cast<float>(sandBox->getSize().x) / 2.F)), static_cast<int>(std::round(staticBodyPixels[i][j].pos.y - position.y * PPM + static_cast<float>(sandBox->getSize().y) / 2.F))}, {air, {0, 0, 0, 0}, 0});
                    }
                }
            }
        }
    }
    std::vector<pixel_t> canvas = sandBox->getCanvas();
    Vec2<u16> sandBoxSize = sandBox->getSize();

    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            if (rigidBody->isDynamic()) {
                BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
                b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
                b2Rot rotation = b2Body_GetRotation(rigidBody->getBodyId());

                transformComponent.position = {(position.x * 8.F * PPM + 1920 / 2.F), -(position.y * 8.F * PPM - 1080 / 2.F)};
                transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / b2_pi;
                std::vector<std::vector<pixel>> dynamicBodyPixels = rigidBody->getRotatedPixels();
            }
        }
    }
}
