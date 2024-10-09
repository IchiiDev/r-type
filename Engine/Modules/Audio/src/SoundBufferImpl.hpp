#pragma once

#include "Rte/Audio/SoundBuffer.hpp"

#include "SFML/Audio/SoundBuffer.hpp"

#include <cstdlib>

namespace Rte::Audio {

    class SoundBufferImpl : public SoundBuffer {
        public:
            SoundBufferImpl() = delete;
            SoundBufferImpl(const void* data, size_t size);
            SoundBufferImpl(const char* path);
            ~SoundBufferImpl() override = default;

            SoundBufferImpl(const SoundBufferImpl&) = delete;
            SoundBufferImpl& operator=(const SoundBufferImpl&) = delete;
            SoundBufferImpl(SoundBufferImpl&&) noexcept = default;
            SoundBufferImpl& operator=(SoundBufferImpl&&) noexcept = default;

            void loadFromMemory(const void* data, size_t size) override;
            void loadFromFile(const char* path) override;

            const sf::SoundBuffer& getBuffer() const { return m_buffer; }

        private:
            sf::SoundBuffer m_buffer;
    };

}   // namespace Rte::Audio
