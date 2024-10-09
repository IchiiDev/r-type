#pragma once

#include "Rte/Common.hpp"
#include "PhysicsSystem.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/PhysicsModule.hpp"
#include "Rte/Physics/ShapeBody.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/SandBox.hpp"

#include "box2d/id.h"

#include <memory>
#include <vector>

namespace Rte::Physics {

    class PhysicsModuleImpl : public PhysicsModule{
        public:
            PhysicsModuleImpl();
            ~PhysicsModuleImpl() override;

            PhysicsModuleImpl(const PhysicsModuleImpl&) = delete;
            PhysicsModuleImpl& operator=(const PhysicsModuleImpl&) = delete;

            PhysicsModuleImpl(PhysicsModuleImpl&&) = default;
            PhysicsModuleImpl& operator=(PhysicsModuleImpl&&) = default;


            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

            [[nodiscard]] std::shared_ptr<RigidBody> createRigidBody(const u8* pixels, const Vec2<u16>& size, const Vec2<float>& pos, float rotation) override;
            [[nodiscard]] std::shared_ptr<RigidBody> createRigidBody(const std::shared_ptr<RigidBody>& rigidBody, const u8* pixels, const Rte::Vec2<u16>& size) override;
            [[nodiscard]] std::shared_ptr<ShapeBody> createShapeBody(const Vec2<u16>& size, float density, float friction, const Vec2<float>& pos, float rotation, bool fixedRotation) override;
            [[nodiscard]] std::shared_ptr<SandBox> createSandBox(const Vec2<u16>& size) override;

            [[nodiscard]] std::vector<u8> fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) override;
            void changeSandBoxPixel(Entity sandBox, const Vec2<int>& pos, const Pixel& pixel) override;
            void applyForce(const std::shared_ptr<ShapeBody>& ShapeBody, const Vec2<float>& force) override;
            void move(const std::shared_ptr<ShapeBody>& ShapeBody, const Vec2<float>& direction) override;

            [[nodiscard]] const std::vector<Pixel>& getSandBoxCanvas(const std::shared_ptr<SandBox>& sandBox) const override;
            [[nodiscard]] const std::vector<Particle>& getSandBoxParticles(const std::shared_ptr<SandBox>& sandBox) const override;

            [[nodiscard]] bool colliding(const std::shared_ptr<ShapeBody> &shapeBody) const override;
            [[nodiscard]] bool colliding(const std::shared_ptr<ShapeBody> &shapeBody1, const std::shared_ptr<ShapeBody> &shapeBody2) const override;

        private:
            std::shared_ptr<Ecs> m_ecs;
            std::shared_ptr<PhysicsSystem> m_physicsSystem;

            b2WorldId m_worldId{};
            float m_timeStep;
            int m_subStepCount;
    };

}   // namespace Rte::Physics
