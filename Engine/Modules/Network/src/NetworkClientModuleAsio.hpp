/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/

#pragma once

#include "Rte/Network/NetworkModuleClient.hpp"
#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"
#include "NetworkModuleImpl.hpp"

#include <memory>
#include <sys/types.h>

namespace Rte::Network {
    class NetworkClientModuleAsio : public NetworkModuleClient {
        public:
            NetworkClientModuleAsio() = default;
            ~NetworkClientModuleAsio() = default;
        
        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;
        
        public:
            void connect(const std::string& host, const unsigned int& port);

        private:
            std::shared_ptr<Ecs> m_ecs;
        
        private:
            std::unique_ptr<CustomClient> m_client = nullptr;
    };
} // namespace Rte::Network
