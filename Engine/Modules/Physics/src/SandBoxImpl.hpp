#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/SandBox.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "Rte/Physics/Tool.hpp"

#include <array>
#include <string>
#include <vector>

namespace Rte::Physics {

    class SandBoxImpl : public SandBox {
        public:
            SandBoxImpl(Vec2<u16> size);
            ~SandBoxImpl();

            SandBoxImpl(const SandBoxImpl&) = delete;
            SandBoxImpl& operator=(const SandBoxImpl&) = delete;

            SandBoxImpl(SandBoxImpl&&) noexcept = default;
            SandBoxImpl& operator=(SandBoxImpl&&) noexcept = default;

            void Step();
            [[nodiscard]] std::vector<pixel_t> getCanvas() const;
            void changePixel(Vec2<int> pos, pixel_t pixel);
        private:
            Rte::Vec2<u16> m_size;
            std::vector<pixel_t> m_canvas;

    };

}   // namespace Rte::Physics
