#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "PhysicsSystem.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include <memory>
#include <string>

namespace Rte::Physics {

    class PhysicsModuleImpl : public PhysicsModule{
        public:
            PhysicsModuleImpl();
            ~PhysicsModuleImpl();
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

            std::shared_ptr<RigidBody> createRigidBody(BodyType type, const u8* pixels, Rte::Vec2<u16> size, float density, float friction, Vec2<float> pos, Vec2<float> scale, float rotation) override;
            std::shared_ptr<RigidBody> createRigidBody(std::shared_ptr<RigidBody> rigidBody, const u8* pixels, Rte::Vec2<u16> size) override;
            void destroyRigidBody(std::shared_ptr<RigidBody>& rigidBody) override;
            u8 *fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) override;

            std::shared_ptr<PlayerBody> createPlayerBody(Vec2<u16> size, float density, float friction, Vec2<float> pos, Vec2<float> scale, float rotation) override;
            void applyForce(std::shared_ptr<PlayerBody> playerBody, Vec2<float> force) override;
        private:
            std::shared_ptr<Ecs> m_ecs;
            std::shared_ptr<PhysicsSystem> m_physicsSystem;

            b2WorldId m_worldId{};
            float m_timeStep;
            int m_subStepCount;
    };

}   // namespace Rte::Physics
