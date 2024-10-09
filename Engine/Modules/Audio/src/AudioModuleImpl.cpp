#include "AudioModuleImpl.hpp"
#include "Rte/Audio/AudioModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "SoundBufferImpl.hpp"
#include "SoundImpl.hpp"

#include <memory>

using namespace Rte::Audio;

Rte::IModule *createModule() {
    return new AudioModuleImpl(); // NOLINT(cppcoreguidelines-owning-memory)
}

void AudioModuleImpl::init(const std::shared_ptr<Ecs>& ecs) {
}

void AudioModuleImpl::update() {
}

std::shared_ptr<SoundBuffer> AudioModuleImpl::createSoundBuffer(const void* data, size_t size) {
    return std::make_shared<SoundBufferImpl>(data, size);
}

std::shared_ptr<SoundBuffer> AudioModuleImpl::createSoundBuffer(const char* path) {
    return std::make_shared<SoundBufferImpl>(path);
}

std::shared_ptr<Sound> AudioModuleImpl::createSound(const std::shared_ptr<SoundBuffer>& soundBuffer) {
    std::shared_ptr<Sound> sound = std::make_shared<SoundImpl>();
    sound->setBuffer(soundBuffer);
    return sound;
}
