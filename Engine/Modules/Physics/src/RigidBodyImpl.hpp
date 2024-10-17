#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/RigidBody.hpp"
#include "Rte/Physics/Tool.hpp"

#include "box2d/id.h"

#include <memory>
#include <vector>

namespace Rte::Physics {

    class RigidBodyImpl : public RigidBody {
        public:
            RigidBodyImpl(const u8* pixels, const Vec2<u16>& size, const b2WorldId& worldId, const Vec2<float>& pos, float rotation);
            RigidBodyImpl(const std::shared_ptr<RigidBodyImpl>& rigidBody, const u8* pixels, const Rte::Vec2<u16>& size, const b2WorldId& worldId);
            ~RigidBodyImpl() override;

            RigidBodyImpl(const RigidBodyImpl&) = delete;
            RigidBodyImpl& operator=(const RigidBodyImpl&) = delete;

            RigidBodyImpl(RigidBodyImpl&&) noexcept = default;
            RigidBodyImpl& operator=(RigidBodyImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
            [[nodiscard]] float getRotation() const;
            [[nodiscard]] Vec2<float> getPosition() const;
            [[nodiscard]] bool isDynamic() const;
            [[nodiscard]] std::vector<std::vector<PixelCringe>> getRotatedPixels() const;
            [[nodiscard]] std::vector<b2ShapeId> getShapesIds() const;
        private:
            bool m_isDynamic;
            b2BodyId m_bodyId{};
            std::vector<u8> m_pixels;
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;
            std::vector<MaterialType> m_materials;
            std::vector<b2ShapeId> m_shapes;
    };

}   // namespace Rte::Physics
