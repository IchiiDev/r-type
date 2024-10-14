#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/ShapeBody.hpp"

#include "box2d/id.h"

namespace Rte::Physics {

    class ShapeBodyImpl : public ShapeBody {
        public:
            ShapeBodyImpl(const Rte::Vec2<u16>& size, float density, float friction, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, bool fixedRotation, bool isStatic, ShapeType shapeType);
            ~ShapeBodyImpl() override;

            ShapeBodyImpl(const ShapeBodyImpl&) = delete;
            ShapeBodyImpl& operator=(const ShapeBodyImpl&) = delete;

            ShapeBodyImpl(ShapeBodyImpl&&) noexcept = default;
            ShapeBodyImpl& operator=(ShapeBodyImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
            [[nodiscard]] b2ShapeId getShapeId() const;
            void applyForce(const Vec2<float>& force);
            void move(const Vec2<float>& direction);

        private:
            b2BodyId m_bodyId{};
            b2ShapeId m_shapeId{};
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;
    };

}   // namespace Rte::Physics
