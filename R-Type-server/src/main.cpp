#include <cstdlib>
#include <exception>
#include <iostream>

#include "ServerApp.hpp"

int main() {
    try {
        ServerApp serverApp;
        serverApp.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
