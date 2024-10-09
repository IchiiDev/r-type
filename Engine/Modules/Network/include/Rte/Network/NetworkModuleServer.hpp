/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleServer
*/

#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"


namespace Rte::Network {
    class NetworkModuleServer : public IModule {
        public:
            NetworkModuleServer();
            ~NetworkModuleServer();
        
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;
    };
} // namespace Rte::Network

extern "C" EXPORT Rte::IModule* createModule();