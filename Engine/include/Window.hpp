#pragma once

#include <memory>

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
