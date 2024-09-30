#pragma once

namespace Rte::Physics {

    class PlayerBody {
        public:
            PlayerBody() = default;
            virtual ~PlayerBody() = default;

            PlayerBody(const PlayerBody&) = delete;
            PlayerBody& operator=(const PlayerBody&) = delete;
            PlayerBody(PlayerBody&&) noexcept = default;
            PlayerBody& operator=(PlayerBody&&) noexcept = default;
    };

}   // namespace Rte::Physics
