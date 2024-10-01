#pragma once

#include "Rte/Common.hpp"

#include <string>
#include <vector>

namespace Rte::Physics {

    enum BodyType: u8 {
	    // zero mass, zero velocity, may be manually moved
	    STATIC = 0,

	    // zero mass, velocity set by user, moved by solver
	    KINEMATIC = 1,

	    // positive mass, velocity determined by forces, moved by solver
	    DYNAMIC = 2,
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
