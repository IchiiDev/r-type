#include "DemoApp.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    try {
        DemoApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
