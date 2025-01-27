#pragma once

#include "Rte/Audio/SoundBuffer.hpp"

#include <cstdlib>
#include <memory>

namespace Rte::Audio {

    class Sound {
        public:
            Sound() = default;
            virtual ~Sound() = default;

            Sound(const Sound&) = delete;
            Sound& operator=(const Sound&) = delete;
            Sound(Sound&&) noexcept = default;
            Sound& operator=(Sound&&) noexcept = default;

            virtual void setBuffer(const std::shared_ptr<SoundBuffer>& buffer) = 0;

            virtual void play() = 0;
            virtual void pause() = 0;
            virtual void stop() = 0;
            virtual void loop(bool loop) = 0;
    };

}   // namespace Rte::Audio
