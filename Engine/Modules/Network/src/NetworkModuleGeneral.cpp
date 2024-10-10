/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleGeneral
*/

#include "NetworkModuleGeneral.hpp"
#include "Rte/Network/NetworkModule.hpp"

Rte::IModule *createModule() {
    return new Rte::Network::NetworkModuleGeneral(); // NOLINT(cppcoreguidelines-owning-memory)
}
