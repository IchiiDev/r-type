#include "PhysicsModuleImpl.hpp"
#include "PlayerBodyImpl.hpp"
#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/Components.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "RigidBodyImpl.hpp"
#include "Rte/Physics/SandBox.hpp"
#include "SandBoxImpl.hpp"
#include "Rte/Physics/Tool.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace Rte::Physics;

Rte::IModule *createModule() {
    return new PhysicsModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

PhysicsModuleImpl::PhysicsModuleImpl() : m_timeStep(1.0F / 60.0F), m_subStepCount(4) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2(0.0F, -9.81F);
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


    // Set la signature du système (déclare les components dont il a besoin)
    Signature signature;
    signature.set(ecs->getComponentType<Components::Physics>());
    signature.set(ecs->getComponentType<BasicComponents::Transform>());
    ecs->setSystemSignature<PhysicsSystem>(signature);
}

void PhysicsModuleImpl::update() {
    m_physicsSystem->update();
}

Rte::u8 *PhysicsModuleImpl::fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) {
    auto rigidBodyImpl = std::dynamic_pointer_cast<RigidBodyImpl>(rigidBody);
    if (!rigidBodyImpl) {
        std::cerr << "Failed to cast RigidBody to RigidBodyImpl" << std::endl;
        return {};
    }
    pixelPos.x = static_cast<int>(std::round(pixelPos.x / 8.F));
    pixelPos.y = static_cast<int>(std::round(pixelPos.y / 8.F));
    Vec2<float> bodyPos = rigidBodyImpl->getPosition();
    bodyPos.x = (bodyPos.x * 8 * PPM) + 1920 / 2;
    bodyPos.y = (-bodyPos.y * 8 * PPM) + 1080 / 2;
    bodyPos.x = std::round(bodyPos.x / 8.F);
    bodyPos.y = std::round(bodyPos.y / 8.F);

    std::vector<std::vector<pixel>> rotatedPixels = rigidBodyImpl->getRotatedPixels();
    for (int i = 0; i < rotatedPixels.size(); i++) {
        for (int j = 0; j < rotatedPixels[i].size(); j++) {
            if (rotatedPixels[i][j].pos.x + bodyPos.x - 2 <= pixelPos.x && rotatedPixels[i][j].pos.x + bodyPos.x + 2 >= pixelPos.x
            && rotatedPixels[i][j].pos.y + bodyPos.y - 2 <= pixelPos.y && rotatedPixels[i][j].pos.y + bodyPos.y + 2 >= pixelPos.y) {
                if (rotatedPixels[i][j].a == 255) {
                    hasChanged = true;
                    rotatedPixels[i][j].a = 0;
                }
            }
        }
    }
    u8 *newPixels = new u8[rotatedPixels.size() * rotatedPixels[0].size() * 4];
    for (int i = 0; i < rotatedPixels.size(); i++) {
        for (int j = 0; j < rotatedPixels[i].size(); j++) {
            newPixels[(i * rotatedPixels[i].size() + j) * 4] = rotatedPixels[i][j].r;
            newPixels[(i * rotatedPixels[i].size() + j) * 4 + 1] = rotatedPixels[i][j].g;
            newPixels[(i * rotatedPixels[i].size() + j) * 4 + 2] = rotatedPixels[i][j].b;
            newPixels[(i * rotatedPixels[i].size() + j) * 4 + 3] = rotatedPixels[i][j].a;
        }
    }
    return newPixels;
}

std::shared_ptr<RigidBody> PhysicsModuleImpl::createRigidBody(const u8* pixels, Vec2<u16> size, Vec2<float> pos, Vec2<float> scale, float rotation) {
    std::shared_ptr<RigidBodyImpl> rigidBody = std::make_shared<RigidBodyImpl>(pixels, size, m_worldId, pos, scale, rotation);
    return rigidBody;
}

std::shared_ptr<RigidBody> PhysicsModuleImpl::createRigidBody(std::shared_ptr<RigidBody> rigidBody, const u8* pixels, Rte::Vec2<u16> size) {
    auto rigidBodyImpl = std::dynamic_pointer_cast<RigidBodyImpl>(rigidBody);
    std::shared_ptr<RigidBodyImpl> newRigidBody = std::make_shared<RigidBodyImpl>(rigidBodyImpl, pixels, size, m_worldId);
    return newRigidBody;
}

void PhysicsModuleImpl::destroyRigidBody(std::shared_ptr<RigidBody>& rigidBody) {
    rigidBody.reset();
}

std::shared_ptr<PlayerBody> PhysicsModuleImpl::createPlayerBody(Rte::Vec2<Rte::u16> size, float density, float friction, Rte::Vec2<float> pos, Rte::Vec2<float> scale, float rotation) {
    std::shared_ptr<PlayerBodyImpl> playerBody = std::make_shared<PlayerBodyImpl>(size, density, friction, m_worldId, pos, scale, rotation);
    return playerBody;
}

void PhysicsModuleImpl::applyForce(std::shared_ptr<PlayerBody> playerBody, Vec2<float> force) {
    auto playerBodyImpl = std::dynamic_pointer_cast<PlayerBodyImpl>(playerBody);
    playerBodyImpl->applyForce(force);
}

std::shared_ptr<SandBox> PhysicsModuleImpl::createSandBox(Vec2<u16> size) {
    return std::make_shared<SandBoxImpl>(size);
}

std::vector<pixel_t> PhysicsModuleImpl::getSandBoxCanvas(std::shared_ptr<SandBox> sandBox) const {
    auto sandBoxImpl = std::dynamic_pointer_cast<SandBoxImpl>(sandBox);
    if (sandBoxImpl) {
        return sandBoxImpl->getCanvas();
    }
    return {};
}

std::vector<particle_t> PhysicsModuleImpl::getSandBoxParticles(std::shared_ptr<SandBox> sandBox) const {
    auto sandBoxImpl = std::dynamic_pointer_cast<SandBoxImpl>(sandBox);
    if (sandBoxImpl) {
        return sandBoxImpl->getParticles();
    }
    return {};
}

void PhysicsModuleImpl::changeSandBoxPixel(Entity sandBox, Vec2<int> pos, pixel_t pixel) {
    auto sandBoxImpl = std::dynamic_pointer_cast<SandBoxImpl>(m_ecs->getComponent<Components::Physics>(sandBox).sandBox);
    if (sandBoxImpl) {
        sandBoxImpl->changePixel(pos, pixel);
    }
}