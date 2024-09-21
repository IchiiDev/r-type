#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"

#include <string>
#include <vector>

namespace Rte::Physics {

    class RigidBodyImpl : public RigidBody {
        public:
            RigidBodyImpl(BodyType type, const std::vector<u8>& pixels, float density, float friction, b2WorldId worldId, Vec2<float> pos, Vec2<float> size);
            ~RigidBodyImpl() override = default;

            RigidBodyImpl(const RigidBodyImpl&) = delete;
            RigidBodyImpl& operator=(const RigidBodyImpl&) = delete;

            RigidBodyImpl(RigidBodyImpl&&) noexcept = default;
            RigidBodyImpl& operator=(RigidBodyImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
        private:
            b2BodyId m_bodyId;
            BodyType m_type;
            std::vector<u8> m_pixels;
            float m_density;
            float m_friction;
    };

}   // namespace Rte::Physics
