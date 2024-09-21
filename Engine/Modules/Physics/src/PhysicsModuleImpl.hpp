#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "PhysicsSystem.hpp"

#include <memory>
#include <string>

namespace Rte::Physics {

    class PhysicsModuleImpl : public PhysicsModule{
        public:
            PhysicsModuleImpl();
            ~PhysicsModuleImpl();
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;
            std::shared_ptr<RigidBody> createRigidBody(BodyType type, const std::vector<u8>& pixels, float density, float friction, Vec2<float> pos, Vec2<float> size) override;

        private:
            std::shared_ptr<Ecs> m_ecs;
            std::shared_ptr<PhysicsSystem> m_physicsSystem;

            b2WorldId m_worldId{};
            float m_timeStep;
            int m_subStepCount;
    };

}   // namespace Rte::Physics
