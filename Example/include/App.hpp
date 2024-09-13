#pragma once

#include "Window.hpp"
#include "Ecs/Coordinator.hpp"

#include <memory>

class App {
    public:
        App();
        ~App() = default;

        void run();

    private:
        std::unique_ptr<Window> m_window;
        bool m_running = true;
};
