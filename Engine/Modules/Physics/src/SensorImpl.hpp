#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/Sensor.hpp"

#include "box2d/id.h"

namespace Rte::Physics {

    class SensorImpl : public Sensor {
        public:
            SensorImpl(const Rte::Vec2<u16>& size, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, SensorType shapeType);
            ~SensorImpl() override;

            SensorImpl(const SensorImpl&) = delete;
            SensorImpl& operator=(const SensorImpl&) = delete;

            SensorImpl(SensorImpl&&) noexcept = default;
            SensorImpl& operator=(SensorImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
            [[nodiscard]] Rte::Vec2<float> getVelocity() const;
            [[nodiscard]] b2ShapeId getShapeId() const;
        private:
            b2BodyId m_bodyId{};
            b2ShapeId m_shapeId{};
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;
    };

}   // namespace Rte::Physics
