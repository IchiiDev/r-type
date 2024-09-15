#pragma once

#include "Window.hpp"

#include <memory>

class App {
    public:
        App();
        ~App() = default;

        void run();

    private:
        static void ecsExample();

    private:
        std::unique_ptr<Rte::Window> m_window;
        bool m_running = true;
};
