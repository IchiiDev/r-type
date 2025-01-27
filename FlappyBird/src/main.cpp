/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** main
*/

#include "FlappyBird.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    try {
        FlappyBird app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
