#include "PhysicsModuleImpl.hpp"
#include "PhysicsSystem.hpp"
#include "ShapeBodyImpl.hpp"
#include "RigidBodyImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/ShapeBody.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/SandBox.hpp"
#include "Rte/Physics/Tool.hpp"
#include "SandBoxImpl.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/math_functions.h"
#include "box2d/types.h"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

using namespace Rte::Physics;

Rte::IModule *createModule() {
    return new PhysicsModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

PhysicsModuleImpl::PhysicsModuleImpl() : m_timeStep(1.0F / 60.0F), m_subStepCount(4) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2{0.0F, -9.81F};
    m_worldId = b2CreateWorld(&worldDef);
}

PhysicsModuleImpl::~PhysicsModuleImpl() {
    b2DestroyWorld(m_worldId);
}

void PhysicsModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
    m_ecs = ecs;

    // Register the RigidBody component as an ecs component
    ecs->registerComponent<Components::Physics>();

    // Physics system registration
    m_physicsSystem = ecs->registerSystem<PhysicsSystem>();
    m_physicsSystem->init(ecs, m_worldId, m_timeStep, m_subStepCount);

    // Set the system's signature
    Signature signature;
    signature.set(ecs->getComponentType<Components::Physics>());
    signature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<PhysicsSystem>(signature);
}

void PhysicsModuleImpl::update() {
    m_physicsSystem->update();
}

std::vector<Rte::u8> PhysicsModuleImpl::fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) {
    const std::shared_ptr<RigidBodyImpl> rigidBodyImpl = interfaceCast<RigidBodyImpl>(rigidBody);


    // Clip pixelPos to the size of a pixel
    pixelPos.x = static_cast<int>(std::round(pixelPos.x / 8));
    pixelPos.y = static_cast<int>(std::round(pixelPos.y / 8));


    // Convert pixelPos to bodyPos
    Vec2<float> bodyPos = rigidBodyImpl->getPosition();
    bodyPos.x = (bodyPos.x * 8 * PPM) + 1920.F / 2;
    bodyPos.y = (-bodyPos.y * 8 * PPM) + 1080.F / 2;
    bodyPos.x = std::round(bodyPos.x / 8.F);
    bodyPos.y = std::round(bodyPos.y / 8.F);


    // IDK
    std::vector<std::vector<PixelCringe>> rotatedPixels = rigidBodyImpl->getRotatedPixels();
    for (std::vector<PixelCringe>& rotatedPixelArray : rotatedPixels) {
        for (PixelCringe& rotatedPixel : rotatedPixelArray) {
            if (rotatedPixel.pos.x + bodyPos.x - 3 <= static_cast<float>(pixelPos.x) && rotatedPixel.pos.x + bodyPos.x + 3 >= static_cast<float>(pixelPos.x)
            && rotatedPixel.pos.y + bodyPos.y - 3 <= static_cast<float>(pixelPos.y) && rotatedPixel.pos.y + bodyPos.y + 3 >= static_cast<float>(pixelPos.y)) {
                if (rotatedPixel.a == 255) {
                    hasChanged = true;
                    rotatedPixel.a = 0;
                }
            }
        }
    }


    // Create the new fractured pixels array
    std::vector<u8> newPixels(rotatedPixels.size() * rotatedPixels.at(0).size() * 4);
    for (u32 i = 0; i < rotatedPixels.size(); i++) {
        for (u32 j = 0; j < rotatedPixels.at(i).size(); j++) {
            newPixels.at((i * rotatedPixels.at(i).size() + j) * 4) = rotatedPixels.at(i).at(j).r;
            newPixels.at((i * rotatedPixels.at(i).size() + j) * 4 + 1) = rotatedPixels.at(i).at(j).g;
            newPixels.at((i * rotatedPixels.at(i).size() + j) * 4 + 2) = rotatedPixels.at(i).at(j).b;
            newPixels.at((i * rotatedPixels.at(i).size() + j) * 4 + 3) = rotatedPixels.at(i).at(j).a;
        }
    }

    return newPixels;
}

std::shared_ptr<RigidBody> PhysicsModuleImpl::createRigidBody(const u8* pixels, const Vec2<u16>& size, const Vec2<float>& pos, float rotation) {
    return std::make_shared<RigidBodyImpl>(pixels, size, m_worldId, pos, rotation);
}

std::shared_ptr<RigidBody> PhysicsModuleImpl::createRigidBody(const std::shared_ptr<RigidBody>& rigidBody, const u8* pixels, const Rte::Vec2<u16>& size) {
    return std::make_shared<RigidBodyImpl>(interfaceCast<RigidBodyImpl>(rigidBody), pixels, size, m_worldId);
}

std::shared_ptr<ShapeBody> PhysicsModuleImpl::createShapeBody(const Rte::Vec2<Rte::u16>& size, float density, float friction, const Rte::Vec2<float>& pos, float rotation, bool fixedRotation, bool isStatic, ShapeType shapeType) {
    return std::make_shared<ShapeBodyImpl>(size, density, friction, m_worldId, pos, rotation, fixedRotation, isStatic, shapeType);
}

void PhysicsModuleImpl::applyForce(const std::shared_ptr<ShapeBody>& ShapeBody, const Vec2<float>& force) {
    interfaceCast<ShapeBodyImpl>(ShapeBody)->applyForce(force);
}

void PhysicsModuleImpl::move(const std::shared_ptr<ShapeBody>& ShapeBody, const Vec2<float>& direction) {
    interfaceCast<ShapeBodyImpl>(ShapeBody)->move(direction);
}

std::shared_ptr<SandBox> PhysicsModuleImpl::createSandBox(const Vec2<u16>& size) {
    return std::make_shared<SandBoxImpl>(size);
}

const std::vector<Pixel>& PhysicsModuleImpl::getSandBoxCanvas(const std::shared_ptr<SandBox>& sandBox) const {
    return interfaceCast<SandBoxImpl>(sandBox)->getCanvas();
}

const std::vector<Particle>& PhysicsModuleImpl::getSandBoxParticles(const std::shared_ptr<SandBox>& sandBox) const {
    return interfaceCast<SandBoxImpl>(sandBox)->getParticles();
}

void PhysicsModuleImpl::changeSandBoxPixel(Entity sandBox, const Vec2<int>& pos, const Pixel& pixel) {
    interfaceCast<SandBoxImpl>(m_ecs->getComponent<Components::Physics>(sandBox).sandBox)->changePixel(pos, pixel);
}

bool PhysicsModuleImpl::colliding(const std::shared_ptr<ShapeBody> &shapeBody) const {
    const std::shared_ptr<ShapeBodyImpl> &shapeBodyImpl = interfaceCast<ShapeBodyImpl>(shapeBody);

    std::vector<b2ContactData> contactData(100);
    b2Body_GetContactData(shapeBodyImpl->getBodyId(), contactData.data(), 100);

    for (const b2ContactData &contact : contactData) {
        if (shapeBodyImpl->getShapeId().index1 == contact.shapeIdA.index1 || shapeBodyImpl->getShapeId().index1 == contact.shapeIdB.index1) {
            return true;
        }
    }
    return false;
}

bool PhysicsModuleImpl::colliding(const std::shared_ptr<ShapeBody> &shapeBody1, const std::shared_ptr<ShapeBody> &shapeBody2) const {
    const std::shared_ptr<ShapeBodyImpl> &shapeBodyImpl1 = interfaceCast<ShapeBodyImpl>(shapeBody1);
    const std::shared_ptr<ShapeBodyImpl> &shapeBodyImpl2 = interfaceCast<ShapeBodyImpl>(shapeBody2);

    std::vector<b2ContactData> contactData(100);
    b2Body_GetContactData(shapeBodyImpl1->getBodyId(), contactData.data(), 100);

    for (const b2ContactData &contact : contactData) {
        if (shapeBodyImpl2->getShapeId().index1 == contact.shapeIdA.index1 || shapeBodyImpl2->getShapeId().index1 == contact.shapeIdB.index1) {
            return true;
        }
    }
    return false;
}
