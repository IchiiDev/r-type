#include "test.hpp"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/WindowEnums.hpp"

#include <optional>

static sf::RenderWindow window;

void createWindow() {
    window.create(sf::VideoMode({800, 600}), "Engine", sf::Style::Default);
    window.setFramerateLimit(60);
}

bool update() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {

        // Window closed
        if (event->is<sf::Event::Closed>()) {
            window.close();
            return false;
        }


        // Escape key
        if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                window.close();
                return false;
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

            window.setView(sf::View(visibleArea));
        }
    }

    sf::RectangleShape rectangle;
    rectangle.setSize({100, 100});
    rectangle.setFillColor(sf::Color(0xFF, 0xFF, 0xFF));
    rectangle.setPosition({200, 200});

    window.clear();
    window.draw(rectangle);
    window.display();

    return true;
}

void destroyWindow() {
    window.close();
}
