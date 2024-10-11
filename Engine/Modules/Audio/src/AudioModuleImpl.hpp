#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Audio/AudioModule.hpp"

#include <memory>

namespace Rte::Audio {

    class AudioModuleImpl : public AudioModule {
        public:
            AudioModuleImpl() = default;
            ~AudioModuleImpl() override = default;

            AudioModuleImpl(const AudioModuleImpl&) = delete;
            AudioModuleImpl& operator=(const AudioModuleImpl&) = delete;
            AudioModuleImpl(AudioModuleImpl&&) noexcept = default;
            AudioModuleImpl& operator=(AudioModuleImpl&&) noexcept = default;

            // Module methods
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

            // Specific methods
            std::shared_ptr<SoundBuffer> createSoundBuffer(const void* data, size_t size) override;
            std::shared_ptr<SoundBuffer> createSoundBuffer(const char* path) override;

            std::shared_ptr<Sound> createSound(const std::shared_ptr<SoundBuffer>& soundBuffer) override;
    };

}   // namespace Rte::Audio
