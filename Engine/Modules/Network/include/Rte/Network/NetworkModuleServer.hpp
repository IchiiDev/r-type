/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** NetworkModuleServer
*/

#pragma once

#include "Rte/Ecs/Types.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleTypes.hpp"


namespace Rte::Network {
    class NetworkModuleServer : public IModule {
        public:
            virtual void updateEntity(const std::shared_ptr<std::vector<Entity>>& entities) = 0;
            virtual void updateTexture(std::map<Entity, PackedTexture>& textures) = 0;
            virtual void sendUpdate() = 0;
            virtual void deleteEntity(BasicComponents::UidComponents id) = 0;
            virtual void deletePlayer(BasicComponents::UidComponents id, uint32_t playerId) = 0;
    };
} // namespace Rte::Network
