#include <cstdlib>
#include <exception>
#include <iostream>

#include "test.hpp"

void run() {
    createWindow();
    while (update());
    destroyWindow();
}

int main() {
    try {
        run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
