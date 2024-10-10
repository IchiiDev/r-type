/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModule
*/

#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "NetworkModuleClient.hpp"
#include "NetworkModuleServer.hpp"
#include "Rte/ModuleManager.hpp"

namespace Rte::Network {
    class NetworkModule : public Rte::IModule{
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;
        
        public:
            [[nodiscard]] virtual std::unique_ptr<Rte::Network::NetworkModuleClient> getClient() const = 0;
            [[nodiscard]] virtual std::unique_ptr<Rte::Network::NetworkModuleServer> getServer() const = 0;
    };
} // namespace Rte::Network

extern "C" EXPORT Rte::IModule* createModule();
