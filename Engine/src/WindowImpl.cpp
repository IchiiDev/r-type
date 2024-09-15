#include "Ecs/Coordinator.hpp"
#include "Ecs/Event.hpp"
#include "Window.hpp"
#include "WindowImpl.hpp"

using namespace Rte;

extern Coordinator coordinator;

std::unique_ptr<Window> Rte::createWindow(int width, int height, const char* title) {
    return std::make_unique<WindowImpl>(width, height, title);
}

WindowImpl::~WindowImpl() {
    m_window.close();
}

void WindowImpl::close() {
    m_window.close();
    coordinator.sendEvent(Events::Window::QUIT);
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
            coordinator.sendEvent(Events::Window::QUIT);
        }

        // Escape key
        if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
                coordinator.sendEvent(Events::Window::QUIT);
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

            Event event(Events::Window::RESIZED);
            event.setParameter(Events::Window::Resized::WIDTH, resized->size.x);
            event.setParameter(Events::Window::Resized::HEIGHT, resized->size.y);
            coordinator.sendEvent(event);

            m_window.setView(sf::View(visibleArea));
        }
    }
}
