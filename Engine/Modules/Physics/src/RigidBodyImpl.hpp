#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/RigidBody.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "Rte/Physics/Tool.hpp"
#include "Rte/Physics/Materials.hpp"

#include <string>
#include <vector>

namespace Rte::Physics {

    class RigidBodyImpl : public RigidBody {
        public:
            RigidBodyImpl(const u8* pixels, Vec2<u16> size, b2WorldId worldId, Vec2<float> pos, Vec2<float> scale, float rotation);
            RigidBodyImpl(std::shared_ptr<RigidBodyImpl> rigidBody, const u8* pixels, Rte::Vec2<u16> size, b2WorldId worldId);
            ~RigidBodyImpl();

            RigidBodyImpl(const RigidBodyImpl&) = delete;
            RigidBodyImpl& operator=(const RigidBodyImpl&) = delete;

            RigidBodyImpl(RigidBodyImpl&&) noexcept = default;
            RigidBodyImpl& operator=(RigidBodyImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
            [[nodiscard]] float getRotation() const;
            [[nodiscard]] Vec2<float> getPosition() const;
            [[nodiscard]] bool isDynamic() const;
            [[nodiscard]] std::vector<std::vector<pixel>> getRotatedPixels() const;
        private:
            bool m_isDynamic;
            b2BodyId m_bodyId;
            const u8* m_pixels;
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;
            materials_t *m_materials;
    };

}   // namespace Rte::Physics
