#include "SoundBufferImpl.hpp"

#include "SFML/Audio/SoundBuffer.hpp"

using namespace Rte::Audio;

SoundBufferImpl::SoundBufferImpl(const void* data, size_t size) {
    loadFromMemory(data, size);
}

SoundBufferImpl::SoundBufferImpl(const char* path) {
    loadFromFile(path);
}

void SoundBufferImpl::loadFromMemory(const void* data, size_t size) {
    if (!m_buffer.loadFromMemory(data, size))
        throw std::runtime_error("Failed to load sound buffer from memory");
}

void SoundBufferImpl::loadFromFile(const char* path) {
    if (!m_buffer.loadFromFile(path))
        throw std::runtime_error("Failed to load sound buffer from file");
}
