#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/PlayerBody.hpp"
#include "Rte/Physics/SandBox.hpp"

#include <memory>
#include <string>

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

            virtual std::shared_ptr<RigidBody> createRigidBody(const u8* pixels, Vec2<u16> size, Vec2<float> pos, Vec2<float> scale, float rotation) = 0;
            virtual std::shared_ptr<RigidBody> createRigidBody(std::shared_ptr<RigidBody> rigidBody, const u8* pixels, Rte::Vec2<u16> size) = 0;
            virtual void destroyRigidBody(std::shared_ptr<RigidBody>& rigidBody) = 0;
            virtual u8 *fractureRigidBody(const std::shared_ptr<RigidBody>& rigidBody, Vec2<u16> pixelPos, bool &hasChanged) = 0;
            virtual std::shared_ptr<PlayerBody> createPlayerBody(Vec2<u16> size, float density, float friction, Vec2<float> pos, Vec2<float> scale, float rotation) = 0;
            virtual void applyForce(std::shared_ptr<PlayerBody> playerBody, Vec2<float> force) = 0;
            
            virtual std::shared_ptr<SandBox> createSandBox(Vec2<u16> size) = 0;
            [[nodiscard]] virtual std::vector<pixel_t> getSandBoxCanvas(std::shared_ptr<SandBox> sandBox) const = 0;
            virtual void changeSandBoxPixel(Entity sandBox, Vec2<int> pos, pixel_t pixel) = 0;
    };
}   // namespace Rte::Physics

extern "C" EXPORT Rte::IModule* createModule();
