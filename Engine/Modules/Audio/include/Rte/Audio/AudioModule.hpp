#pragma once

#include "Rte/Audio/Sound.hpp"
#include "Rte/Audio/SoundBuffer.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

namespace Rte::Audio {

    /**
     * @brief Graphic module interface.
     * This module is responsible for rendering entities to the window.
     * It uses the RenderSystem to render all the entities that have a transform and a sprite component.
     */
    class AudioModule : public IModule {
        public:
            AudioModule() = default;
            ~AudioModule() override = default;

            AudioModule(const AudioModule&) = delete;
            AudioModule& operator=(const AudioModule&) = delete;

            AudioModule(AudioModule&&) = default;
            AudioModule& operator=(AudioModule&&) = default;


        public: // Overridden methods
            /**
             * @brief Initializes the module.
             * /!\ This method must be called before any other method.
             *
             * @param ecs Pointer to the ECS.
             */
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;

            /**
             * @brief Updates the module.
             * This method should be called every frame.
             * It will render all the entities to the window using the RenderSystem.
             */
            void update() override = 0;


        public: // Specific methods
            /**
             * @brief Create a Sound buffer from memory.
             * The data must be a valid sound file.
             *
             * @param data Pointer to the data.
             * @param size Size of the data.
             * @return std::shared_ptr<SoundBuffer> Pointer to the created SoundBuffer.
             */
            virtual std::shared_ptr<SoundBuffer> createSoundBuffer(const void* data, size_t size) = 0;

            /**
             * @brief Create a Sound buffer from a file.
             * The file must be a valid sound file.
             *
             * @param path Path to the file.
             * @return std::shared_ptr<SoundBuffer> Pointer to the created SoundBuffer.
             */
            virtual std::shared_ptr<SoundBuffer> createSoundBuffer(const char* path) = 0;

            /**
             * @brief Create a Sound from a SoundBuffer.
             *
             * @param soundBuffer SoundBuffer to attach to the Sound.
             * @return std::shared_ptr<Sound> Pointer to the created Sound.
             */
            virtual std::shared_ptr<Sound> createSound(const std::shared_ptr<SoundBuffer>& soundBuffer) = 0;
    };

}   // namespace Rte::Audio

extern "C" EXPORT Rte::IModule* createModule();
