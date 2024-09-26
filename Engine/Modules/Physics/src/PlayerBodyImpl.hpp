#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/PlayerBody.hpp"

#include "box2d/box2d.h"
#include "box2d/id.h"
#include "Rte/Physics/Tool.hpp"

#include <string>
#include <vector>

namespace Rte::Physics {

    class PlayerBodyImpl : public PlayerBody {
        public:
            PlayerBodyImpl(Rte::Vec2<u16> size, float density, float friction, b2WorldId worldId, Vec2<float> pos, Vec2<float> scale, float rotation);
            ~PlayerBodyImpl();

            PlayerBodyImpl(const PlayerBodyImpl&) = delete;
            PlayerBodyImpl& operator=(const PlayerBodyImpl&) = delete;

            PlayerBodyImpl(PlayerBodyImpl&&) noexcept = default;
            PlayerBodyImpl& operator=(PlayerBodyImpl&&) noexcept = default;
            [[nodiscard]] b2BodyId getBodyId() const;
            void applyForce(Vec2<float> force);
        private:
            b2BodyId m_bodyId;
            Rte::Vec2<u16> m_size;
            b2WorldId m_worldId;

    };

}   // namespace Rte::Physics
