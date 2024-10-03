#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/PlayerBody.hpp"

#include "box2d/id.h"

namespace Rte::Physics {

    class PlayerBodyImpl : public PlayerBody {
        public:
            PlayerBodyImpl(const Rte::Vec2<u16>& size, float density, float friction, const b2WorldId& worldId, const Vec2<float>& pos, float rotation, bool fixedRotation);
            ~PlayerBodyImpl() override;

            PlayerBodyImpl(const PlayerBodyImpl&) = delete;
            PlayerBodyImpl& operator=(const PlayerBodyImpl&) = delete;

            PlayerBodyImpl(PlayerBodyImpl&&) noexcept = default;
            PlayerBodyImpl& operator=(PlayerBodyImpl&&) noexcept = default;

            [[nodiscard]] b2BodyId getBodyId() const;
            void applyForce(const Vec2<float>& force);
            void move(const Vec2<float>& direction);

        private:
            b2BodyId m_bodyId{};
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;
    };

}   // namespace Rte::Physics
