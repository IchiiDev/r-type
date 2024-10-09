#pragma once

#include <cstdlib>

namespace Rte::Audio {

    class SoundBuffer {
        public:
            SoundBuffer() = default;
            virtual ~SoundBuffer() = default;

            SoundBuffer(const SoundBuffer&) = delete;
            SoundBuffer& operator=(const SoundBuffer&) = delete;
            SoundBuffer(SoundBuffer&&) noexcept = default;
            SoundBuffer& operator=(SoundBuffer&&) noexcept = default;

            virtual void loadFromMemory(const void* data, size_t size) = 0;
            virtual void loadFromFile(const char* path) = 0;
    };

}   // namespace Rte::Audio