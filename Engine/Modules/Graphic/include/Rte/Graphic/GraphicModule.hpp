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
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;

            virtual void setWindowTitle(const std::string& title) = 0;
            virtual void setWindowSize(const Vec2<u16>& size) = 0;
            [[nodiscard]] virtual Vec2<u16> getWindowSize() const = 0;

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
