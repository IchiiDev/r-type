#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/PlayerBody.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/SandBox.hpp"

#include <memory>

namespace Rte::Physics {

    class PhysicsModule : public IModule {
        public: // Overridden methods
            /**
             * @brief Initializes the module.
             * /!\ This method must be called before any other method.
             *
             * @param ecs Pointer to the ECS.
             */
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;

            /**
             * @brief Updates the module.
             * This method should be called every frame.
             * It will render all the entities to the window using the RenderSystem.
             */
            void update() override = 0;


            [[nodiscard]] virtual std::shared_ptr<RigidBody> createRigidBody(const u8* pixels, const Vec2<u16>& size, const Vec2<float>& pos, float rotation) = 0;
            [[nodiscard]] virtual std::shared_ptr<RigidBody> createRigidBody(const std::shared_ptr<RigidBody>& rigidBody, const u8* pixels, const Rte::Vec2<u16>& size) = 0;
            [[nodiscard]] virtual std::shared_ptr<PlayerBody> createPlayerBody(const Vec2<u16>& size, float density, float friction, const Vec2<float>& pos, float rotation) = 0;
            [[nodiscard]] virtual std::shared_ptr<SandBox> createSandBox(const Vec2<u16>& size) = 0;

            [[nodiscard]] virtual std::vector<u8> fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) = 0;
            virtual void changeSandBoxPixel(Entity sandBox, const Vec2<int>& pos, const Pixel& pixel) = 0;
            virtual void applyForce(const std::shared_ptr<PlayerBody>& playerBody, const Vec2<float>& force) = 0;

            [[nodiscard]] virtual const std::vector<Pixel>& getSandBoxCanvas(const std::shared_ptr<SandBox>& sandBox) const = 0;
            [[nodiscard]] virtual const std::vector<Particle>& getSandBoxParticles(const std::shared_ptr<SandBox>& sandBox) const = 0;
    };

}   // namespace Rte::Physics

extern "C" EXPORT Rte::IModule* createModule();
