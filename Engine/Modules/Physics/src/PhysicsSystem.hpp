#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"

#include "box2d/id.h"

#include <memory>

namespace Rte::Physics {

    class PhysicsSystem : public System {
        public:
            void init(const std::shared_ptr<Rte::Ecs>& ecs, b2WorldId worldId, float timeStep, int subStepCount);
            void update();

        private:
            std::shared_ptr<Rte::Ecs> m_ecs;
            b2WorldId m_worldId{};
            float m_timeStep;
            int m_subStepCount;
    };

}   // namespace Rte::Physics
