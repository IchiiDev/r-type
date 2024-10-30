#pragma once

#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/ShapeBody.hpp"
#include "Rte/Physics/SandBox.hpp"

#include <memory>

namespace Rte::Physics::Components {

    struct Physics {
        std::shared_ptr<RigidBody> rigidBody;
        std::shared_ptr<ShapeBody> shapeBody;
        std::shared_ptr<SandBox> sensor;
        std::shared_ptr<SandBox> sandBox;
    };

}   // namespace Rte::Physics::Components
