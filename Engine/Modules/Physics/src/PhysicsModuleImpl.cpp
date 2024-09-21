#include "PhysicsModuleImpl.hpp"
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

#include <memory>
#include <optional>
#include <string>

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

std::shared_ptr<RigidBody> PhysicsModuleImpl::createRigidBody(BodyType type, const std::vector<Rte::u8>& pixels, float density, float friction, Vec2<float> pos, Vec2<float> size) {
    std::shared_ptr<RigidBodyImpl> rigidBody = std::make_shared<RigidBodyImpl>(type, pixels, density, friction, m_worldId, pos, size);
    return rigidBody;
}
