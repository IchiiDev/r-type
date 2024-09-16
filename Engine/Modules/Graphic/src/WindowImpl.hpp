#pragma once

#include "Window.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

namespace Rte {

    class WindowImpl : public Window {
        public:
            WindowImpl(int width, int height, const char* title);
            ~WindowImpl() override;

            WindowImpl(const WindowImpl&) = delete;
            WindowImpl& operator=(const WindowImpl&) = delete;

            WindowImpl(WindowImpl&&) = default;
            WindowImpl& operator=(WindowImpl&&) = default;

            void close() override;
            void clear() override;
            void display() override;
            void update() override;

        private:
            sf::RenderWindow m_window;
    };

}   // namespace Rte
