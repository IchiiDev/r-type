#pragma once

namespace Rte::Physics {
    enum class SensorType {
        CIRCLE,
        CAPSULE,
        RECTANGLE
    };
    
    class Sensor {
        public:
            Sensor() = default;
            virtual ~Sensor() = default;

            Sensor(const Sensor&) = delete;
            Sensor& operator=(const Sensor&) = delete;
            Sensor(Sensor&&) noexcept = default;
            Sensor& operator=(Sensor&&) noexcept = default;
    };

}   // namespace Rte::Physics
