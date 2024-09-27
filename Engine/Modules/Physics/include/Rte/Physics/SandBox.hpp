#pragma once

#include "Rte/Common.hpp"

#include <string>
#include <vector>

namespace Rte::Physics {
    class SandBox {
        public:
            SandBox() = default;
            virtual ~SandBox() = default;

            SandBox(const SandBox&) = delete;
            SandBox& operator=(const SandBox&) = delete;
            SandBox(SandBox&&) noexcept = default;
            SandBox& operator=(SandBox&&) noexcept = default;
    };

}   // namespace Rte::Physics
