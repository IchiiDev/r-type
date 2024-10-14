#include "PhysicsSystem.hpp"
#include "ShapeBodyImpl.hpp"
#include "RigidBodyImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/Tool.hpp"
#include "SandBoxImpl.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"

#include <cassert>
#include <chrono>
#include <cmath>
#include <memory>
#include <numbers>
#include <vector>

using namespace Rte::Physics;

void PhysicsSystem::init(const std::shared_ptr<Rte::Ecs>& ecs, b2WorldId worldId, float timeStep, int subStepCount) {
    m_ecs = ecs;
    m_worldId = worldId;
    m_timeStep = timeStep;
    m_subStepCount = subStepCount;
}

void PhysicsSystem::update() {  // NOLINT (readability-function-cognitive-complexity)
    assert(m_ecs != nullptr && "Cannot update physics system: Not initialized.");
    
    static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    const std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    
    lastTime = currentTime;

    b2World_Step(m_worldId, deltaTime, m_subStepCount);

    std::shared_ptr<SandBoxImpl> sandBox;
    for (const Entity entity : m_entities)
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).sandBox)
            sandBox = interfaceCast<SandBoxImpl>(m_ecs->getComponent<Components::Physics>(entity).sandBox);

    const std::vector<Pixel> canvas = sandBox->getCanvas();
    const Vec2<u16> sandBoxSize = sandBox->getSize();

    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
            const b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
            const b2Rot rotation = b2Body_GetRotation(rigidBody->getBodyId());

            transformComponent.position = {(position.x * 8.F * PPM), -(position.y * 8.F * PPM)};
            transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / std::numbers::pi_v<float>;
            const std::vector<std::vector<PixelCringe>> staticBodyPixels = rigidBody->getRotatedPixels();

            //Put the rigidbody pixels in the sandBox
            for (const std::vector<Rte::Physics::PixelCringe>& staticBodyPixelArray : staticBodyPixels) {
                for (const Rte::Physics::PixelCringe& staticBodyPixel : staticBodyPixelArray) {
                    if (staticBodyPixel.r != 0 || staticBodyPixel.g != 0 || staticBodyPixel.b != 0) {
                        const Vec2<int> pos = {static_cast<int>(std::round(staticBodyPixel.pos.x + position.x * PPM + static_cast<float>(sandBoxSize.x) / 2.F)), static_cast<int>(std::round(staticBodyPixel.pos.y - position.y * PPM + static_cast<float>(sandBoxSize.y) / 2.F))};
                        if (pos.x >= 0 && pos.x < sandBoxSize.x && pos.y >= 0 && pos.y < sandBoxSize.y && mats.at(canvas.at(pos.y * sandBoxSize.x + pos.x).mat).comp != Comportement::NOTHING)
                            sandBox->addParticle({{canvas.at(pos.y * sandBoxSize.x + pos.x)}, pos, {0, 0}});
                        sandBox->changePixel(pos, {MaterialType::STATIC_WOOD, invMatColors.at(MaterialType::STATIC_WOOD), 0});
                    }
                }
            }
        }

        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).shapeBody) {
            const std::shared_ptr<ShapeBodyImpl>& ShapeBody = interfaceCast<ShapeBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).shapeBody);
            BasicComponents::Transform& transformComponent = m_ecs->getComponent<BasicComponents::Transform>(entity);
            const b2Vec2 position = b2Body_GetPosition(ShapeBody->getBodyId());
            const b2Rot rotation = b2Body_GetRotation(ShapeBody->getBodyId());

            transformComponent.position = {(position.x * 8.F * PPM), -(position.y * 8.F * PPM)};
            transformComponent.rotation = -b2Rot_GetAngle(rotation) * 180 / std::numbers::pi_v<float>;
        }
    }

    if (sandBox)
        sandBox->Step();

    for (const Entity entity : m_entities) {
        if (m_ecs->getComponent<Rte::Physics::Components::Physics>(entity).rigidBody) {
            const std::shared_ptr<RigidBodyImpl>& rigidBody = interfaceCast<RigidBodyImpl>(m_ecs->getComponent<Components::Physics>(entity).rigidBody);
            const b2Vec2 position = b2Body_GetPosition(rigidBody->getBodyId());
            const std::vector<std::vector<PixelCringe>> staticBodyPixels = rigidBody->getRotatedPixels();

            //Remove the rigidbody pixels from the sandBox
            for (const std::vector<Rte::Physics::PixelCringe>& staticBodyPixel : staticBodyPixels) {
                for (const Rte::Physics::PixelCringe& j : staticBodyPixel) {
                    if (j.r != 0 || j.g != 0 || j.b != 0)
                        sandBox->changePixel({static_cast<int>(std::round(j.pos.x + position.x * PPM + static_cast<float>(sandBox->getSize().x) / 2.F)), static_cast<int>(std::round(j.pos.y - position.y * PPM + static_cast<float>(sandBox->getSize().y) / 2.F))}, {MaterialType::AIR, {0, 0, 0, 0}, 0});
                }
            }
        }
    }
}
