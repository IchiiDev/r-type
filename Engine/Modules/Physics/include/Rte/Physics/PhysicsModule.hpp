#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Physics/RigidBody.hpp"

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

            virtual std::shared_ptr<RigidBody> createRigidBody(BodyType type, const std::vector<u8>& pixels, float density, float friction, Vec2<float> pos, Vec2<float> size) = 0;
    };
}   // namespace Rte::Physics

extern "C" EXPORT Rte::IModule* createModule();
