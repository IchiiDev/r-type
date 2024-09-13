#include "App.hpp"
#include "Ecs/Coordinator.hpp"
#include "Ecs/Types.hpp"
#include <cstdio>

Coordinator coordinator;

App::App() {
    m_window = createWindow(800, 600, "Example");

    coordinator.Init();
    coordinator.AddEventListener(LAMBDA_LISTENER(Events::Window::QUIT, [this](const Event& /* UNUSED */) {
        m_running = false;
    }));
}

void App::run() {
    while (m_running) {
        m_window->clear();
        m_window->update();
        m_window->display();
    }
}
