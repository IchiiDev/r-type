/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleGeneral
*/

#pragma once

#include "NetworkClientModuleAsio.hpp"
#include "NetworkServerModuleAsio.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"

#include <memory>

namespace Rte::Network {
    class NetworkModuleGeneral : public Rte::Network::NetworkModule {
        public:
            NetworkModuleGeneral() = default;
            ~NetworkModuleGeneral() = default;

        public:
            void init(const std::shared_ptr<Ecs>& ecs) override {};
            void update() override {};

        public:
            [[nodiscard]] std::unique_ptr<Rte::Network::NetworkModuleClient> getClient() const override { return std::make_unique<Rte::Network::NetworkClientModuleAsio>(); }
            [[nodiscard]] std::unique_ptr<Rte::Network::NetworkModuleServer> getServer() const override { return std::make_unique<Rte::Network::NetworkServerModuleAsio>(); }
    };
} // namespace Rte::Network
