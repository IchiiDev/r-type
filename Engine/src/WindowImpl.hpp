#pragma once

#include "Window.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

namespace Rte {

    class WindowImpl : public Window {
        public:
            WindowImpl(int width, int height, const char* title) {
                m_window.create(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title);
            };

            ~WindowImpl() override;

            void close() override;
            void clear() override;
            void display() override;
            void update() override;

        private:
            sf::RenderWindow m_window;
    };

}   // namespace Rte
