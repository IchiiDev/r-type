#pragma once
#include "Rte/Common.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include <memory>
#include <vector>

namespace Rte::Physics::Components {

    struct Physics {
        std::shared_ptr<RigidBody> rigidBody;
    };
    
}   // namespace Rte::Physics::Components
