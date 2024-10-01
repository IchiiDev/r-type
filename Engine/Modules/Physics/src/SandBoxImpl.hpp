#pragma once

#include "Rte/Common.hpp"
#include "Rte/Physics/Materials.hpp"
#include "Rte/Physics/SandBox.hpp"

#include <vector>

namespace Rte::Physics {

    class SandBoxImpl : public SandBox {
        public:
            SandBoxImpl(Vec2<u16> size);
            ~SandBoxImpl() override = default;

            SandBoxImpl(const SandBoxImpl&) = delete;
            SandBoxImpl& operator=(const SandBoxImpl&) = delete;

            SandBoxImpl(SandBoxImpl&&) noexcept = default;
            SandBoxImpl& operator=(SandBoxImpl&&) noexcept = default;

            void Step();
            void changePixel(const Vec2<int>& pos, const Pixel& pixel);
            void addParticle(const Particle& particle);

            [[nodiscard]] const std::vector<Pixel>& getCanvas();
            [[nodiscard]] const Rte::Vec2<u16>& getSize();
            [[nodiscard]] const std::vector<Particle>& getParticles();

        private:
            Rte::Vec2<u16> m_size;
            std::vector<Pixel> m_canvas;
            std::vector<Particle> m_particles;
    };

}   // namespace Rte::Physics
