#pragma once

#include "Rte/Common.hpp"

namespace Rte::Physics {

    enum BodyType: u8 {
        // zero mass, zero velocity, may be manually moved
        STATIC,

        // zero mass, velocity set by user, moved by solver
        KINEMATIC,

        // positive mass, velocity determined by forces, moved by solver
        DYNAMIC
    };

    class RigidBody {
        public:
            RigidBody() = default;
            virtual ~RigidBody() = default;

            RigidBody(const RigidBody&) = delete;
            RigidBody& operator=(const RigidBody&) = delete;
            RigidBody(RigidBody&&) noexcept = default;
            RigidBody& operator=(RigidBody&&) noexcept = default;
    };

}   // namespace Rte::Physics
