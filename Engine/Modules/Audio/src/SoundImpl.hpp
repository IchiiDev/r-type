#pragma once

#include "Rte/Audio/Sound.hpp"
#include "Rte/Audio/SoundBuffer.hpp"

#include "SFML/Audio/Sound.hpp"

#include <cstdlib>
#include <memory>

namespace Rte::Audio {

    class SoundImpl : public Sound {
        public:
            SoundImpl() = default;
            ~SoundImpl() override = default;

            SoundImpl(const SoundImpl&) = delete;
            SoundImpl& operator=(const SoundImpl&) = delete;
            SoundImpl(SoundImpl&&) noexcept = default;
            SoundImpl& operator=(SoundImpl&&) noexcept = default;

            void setBuffer(const std::shared_ptr<SoundBuffer>& buffer) override;

            void play() override;
            void pause() override;
            void stop() override;

        private:
            std::unique_ptr<sf::Sound> m_sound;
    };

}   // namespace Rte::Audio
