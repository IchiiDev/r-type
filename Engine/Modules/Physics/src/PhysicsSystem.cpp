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
    std::vector<Pixel> canvas = sandBox->getCanvas();
    Vec2<u16> sandBoxSize = sandBox->getSize();
    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
            b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
            b2Rot rotation = b2Body_GetRotation(rigidBody->getBodyId());

            transformComponent.position = {(position.x * 8.F * PPM + 1920 / 2.F), -(position.y * 8.F * PPM - 1080 / 2.F)};
            transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / b2_pi;
            std::vector<std::vector<pixel>> staticBodyPixels = rigidBody->getRotatedPixels();

            //Put the rigidbody pixels in the sandBox
            for (int i = 0; i < staticBodyPixels.size(); i++) {
                for (int j = 0; j < staticBodyPixels[i].size(); j++) {
                    if (staticBodyPixels[i][j].r != 0 || staticBodyPixels[i][j].g != 0 || staticBodyPixels[i][j].b != 0) {
                        Vec2<int> pos = {static_cast<int>(std::round(staticBodyPixels[i][j].pos.x + position.x * PPM + static_cast<float>(sandBoxSize.x) / 2.F)), static_cast<int>(std::round(staticBodyPixels[i][j].pos.y - position.y * PPM + static_cast<float>(sandBoxSize.y) / 2.F))};
                        if (pos.x >= 0 && pos.x < sandBoxSize.x && pos.y >= 0 && pos.y < sandBoxSize.y && mats.at(canvas.at(pos.y * sandBoxSize.x + pos.x).mat).comp != Comportement::NOTHING)
                            sandBox->addParticle({{canvas.at(pos.y * sandBoxSize.x + pos.x)}, pos, {0, 0}});
                        sandBox->changePixel(pos, {MaterialType::STATIC_WOOD, invMatColors.at(MaterialType::STATIC_WOOD), 0});
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
            b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
            std::vector<std::vector<pixel>> staticBodyPixels = rigidBody->getRotatedPixels();

            //Remove the rigidbody pixels from the sandBox
            for (int i = 0; i < staticBodyPixels.size(); i++) {
                for (int j = 0; j < staticBodyPixels[i].size(); j++) {
                    if (staticBodyPixels[i][j].r != 0 || staticBodyPixels[i][j].g != 0 || staticBodyPixels[i][j].b != 0)
                        sandBox->changePixel({static_cast<int>(std::round(staticBodyPixels[i][j].pos.x + position.x * PPM + static_cast<float>(sandBox->getSize().x) / 2.F)), static_cast<int>(std::round(staticBodyPixels[i][j].pos.y - position.y * PPM + static_cast<float>(sandBox->getSize().y) / 2.F))}, {MaterialType::AIR, {0, 0, 0, 0}, 0});
                }
            }
        }
    }
}
