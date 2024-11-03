/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkClientModuleAsio
*/

#pragma once

#include "Rte/Network/NetworkModuleClient.hpp"
#include "NetworkModuleImpl.hpp"
#include "Rte/Network/NetworkModule.hpp"

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
            void connect(const std::string& host, const uint16_t& port) override;
            void updateInputs(PackedInput input) override;
            void sendUpdate() override;

        private:
            PackedInput m_input;

        private:
            std::unique_ptr<CustomClient> m_client = nullptr;

        private:
            std::shared_ptr<Ecs> m_ecs;
    };
} // namespace Rte::Network
