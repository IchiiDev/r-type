/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleClient
*/

#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include <sys/types.h>
#include "NetworkModuleTypes.hpp"

namespace Rte::Network {
    class NetworkModuleClient : public IModule {
        public:
            virtual void connect(const std::string& host, const unsigned int& port) = 0;
            virtual void updateInputs(PackedInput input) = 0;
            virtual void sendUpdate() = 0;
            virtual void connectUdp(const std::string& host, const unsigned int& port,  unsigned int maxTry) = 0;
    };
} // namespace Rte::Network

