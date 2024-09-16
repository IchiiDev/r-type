#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Event.hpp"
#include "Window.hpp"
#include "WindowImpl.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/VideoMode.hpp"

#include <memory>
#include <optional>

using namespace Rte;

extern Ecs ecs; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

std::unique_ptr<Window> Rte::createWindow(int width, int height, const char* title) {
    return std::make_unique<WindowImpl>(width, height, title);
}

WindowImpl::WindowImpl(int width, int height, const char* title) : m_window(sf::VideoMode({static_cast<u32>(width), static_cast<u32>(height)}), title) {
}

WindowImpl::~WindowImpl() {
    m_window.close();
}

void WindowImpl::close() {
    m_window.close();
    ecs.sendEvent(Events::Window::QUIT);
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
            ecs.sendEvent(Events::Window::QUIT);
        }

        // Escape key
        if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
                ecs.sendEvent(Events::Window::QUIT);
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
            ecs.sendEvent(event);

            m_window.setView(sf::View(visibleArea));
        }
    }
}
