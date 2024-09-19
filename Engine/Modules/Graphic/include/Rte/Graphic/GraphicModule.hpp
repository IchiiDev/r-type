#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <string>

namespace Rte::Graphic {

    class GraphicModule : public IModule {
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
             * @brief Sets the title of the window.
             *
             * @param title Title of the window.
             */
            virtual void setWindowTitle(const std::string& title) = 0;

            /**
             * @brief Set the size of the window.
             *
             * @param size Size of the window.
             */
            virtual void setWindowSize(const Vec2<u16>& size) = 0;

            /**
             * @brief Get the size of the window.
             *
             * @return Vec2<u16> Size of the window.
             */
            [[nodiscard]] virtual Vec2<u16> getWindowSize() const = 0;

            /**
             * @brief Create a texture.
             *
             * @return std::shared_ptr<Texture> Shared pointer to the texture.
             */
            [[nodiscard]] virtual std::shared_ptr<Texture> createTexture() const = 0;
    };


    namespace Events::Window {
        const EventType QUIT = "Events::Window::QUIT"_hash;
        const EventType RESIZED = "Events::Window::RESIZED"_hash;
        const EventType INPUT = "Events::Window::INPUT"_hash;
    }   // namespace Events::Window

    namespace Events::Window::Input {
        const ParamId INPUT = "Events::Window::Input::INPUT"_hash;
    }   // namespace Events::Window::Input

    namespace Events::Window::Resized {
        const ParamId WIDTH = "Events::Window::Resized::WIDTH"_hash;
        const ParamId HEIGHT = "Events::Window::Resized::HEIGHT"_hash;
    }   // namespace Events::Window::Resized

}   // namespace Rte::Graphic

extern "C" EXPORT Rte::IModule* createModule();
