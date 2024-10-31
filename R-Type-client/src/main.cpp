#include <cstdlib>
#include <exception>
#include <iostream>

#include "ClientApp.hpp"

int main() {
    try {
        ClientApp clientApp;
        clientApp.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
