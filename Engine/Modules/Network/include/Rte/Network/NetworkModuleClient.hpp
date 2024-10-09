/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleClient
*/

#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"


namespace Rte::Network {
    class NetworkModuleClient : public IModule {
        public:
            NetworkModuleClient();
            ~NetworkModuleClient();

        public:
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;
    };

    namespace Events::Network {
        const EventType DISCONNECTED = "Events::Network::DISCONNECTED"_hash;
    } // namespace Events::Network
} // namespace Rte::Network


extern "C" EXPORT Rte::IModule* createModule();
