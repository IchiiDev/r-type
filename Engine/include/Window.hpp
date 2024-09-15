#pragma once

#include "Ecs/Types.hpp"

#include <memory>

namespace Rte {

    class Window {
        public:
            Window() = default;
            virtual ~Window() = default;

            virtual void close() = 0;
            virtual void clear() = 0;
            virtual void display() = 0;
            virtual void update() = 0;
    };

    std::unique_ptr<Window> createWindow(int width, int height, const char* title);

    namespace Events::Window {
        const EventType QUIT = "Events::Window::QUIT"_hash;
        const EventType RESIZED = "Events::Window::RESIZED"_hash;
    }   // namespace Events::Window

    namespace Events::Window::Resized {
        const ParamId WIDTH = "Events::Window::Resized::WIDTH"_hash;
        const ParamId HEIGHT = "Events::Window::Resized::HEIGHT"_hash;
    }   // namespace Events::Window::Resized

}   // namespace Rte
