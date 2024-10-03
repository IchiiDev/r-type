#pragma once

namespace Rte::Physics {

    class ShapeBody {
        public:
            ShapeBody() = default;
            virtual ~ShapeBody() = default;

            ShapeBody(const ShapeBody&) = delete;
            ShapeBody& operator=(const ShapeBody&) = delete;
            ShapeBody(ShapeBody&&) noexcept = default;
            ShapeBody& operator=(ShapeBody&&) noexcept = default;
    };

}   // namespace Rte::Physics
