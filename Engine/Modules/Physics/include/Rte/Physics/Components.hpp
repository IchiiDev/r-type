#pragma once
#include "Rte/Common.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/PlayerBody.hpp"
#include "Rte/Physics/SandBox.hpp"

#include <memory>
#include <vector>

namespace Rte::Physics::Components {

    struct Physics {
        std::shared_ptr<RigidBody> rigidBody;
        std::shared_ptr<PlayerBody> playerBody;
        std::shared_ptr<SandBox> sandBox;
    };
    
}   // namespace Rte::Physics::Components