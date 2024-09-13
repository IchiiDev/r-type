#include "WindowImpl.hpp"
#include "Ecs/Event.hpp"
#include "Window.hpp"
#include "Ecs/Coordinator.hpp"

extern Coordinator coordinator;

std::unique_ptr<Window> createWindow(int width, int height, const char* title) {
    return std::make_unique<WindowImpl>(width, height, title);
}

WindowImpl::~WindowImpl() {
    m_window.close();
}

void WindowImpl::close() {
    m_window.close();
    coordinator.SendEvent(Events::Window::QUIT);
}

void WindowImpl::clear() {
    m_window.clear();
}

void WindowImpl::display() {
    m_window.display();
}

void WindowImpl::update() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {

        // Window closed
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            coordinator.SendEvent(Events::Window::QUIT);
        }

        // Escape key
        if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
                coordinator.SendEvent(Events::Window::QUIT);
            }
        }


        // Window resize
        if (const sf::Event::Resized* resized = event->getIf<sf::Event::Resized>()) {
            const sf::FloatRect visibleArea(
                sf::Vector2<float>(0, 0),
                sf::Vector2<float>(
                    static_cast<float>(resized->size.x),
                    static_cast<float>(resized->size.y)
                )
            );

            m_window.setView(sf::View(visibleArea));
        }
    }
}
