/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkServerModule
*/

#pragma once

#include "Rte/Common.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"
#include "BetterNetworkLibrary/BetterNetworkLibrary.hpp"
#include "NetworkModuleImpl.hpp"

#include <memory>
#include <sys/types.h>

namespace Rte::Network {
    class NetworkServerModuleAsio : public NetworkModuleServer {
        public:
            NetworkServerModuleAsio() = default;
            ~NetworkServerModuleAsio() = default;

        public:
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;

        public:
            void start(unsigned int port) override;
            void updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) override;
            void updateTexture(std::map<Entity, PackedTexture>& textures) override;

        private:
            std::shared_ptr<Ecs> m_ecs;

        private:
            std::unique_ptr<CustomServer> m_server = nullptr;

        private:
            std::vector<Entity> m_alreadySentEntity;
            std::map<Entity, PackedTexture> m_textures;
            std::shared_ptr<std::vector<Entity>> m_entities = nullptr;
    };
} // namespace Rte::Network
