/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleServer
*/

#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"


namespace Rte::Network {
    class NetworkModuleServer : public IModule {     
        public:
            virtual void start(unsigned int port) = 0;
            virtual void updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) = 0;
    };
} // namespace Rte::Network
