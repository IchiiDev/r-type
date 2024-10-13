#pragma once

#include "Player.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"
#include "Rte/Physics/PhysicsModule.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

class ServerApp {
    public:
        ServerApp();
        ~ServerApp() = default;

        ServerApp(const ServerApp&) = delete;
        ServerApp& operator=(const ServerApp&) = delete;
        ServerApp(ServerApp&&) noexcept = delete;
        ServerApp& operator=(ServerApp&&) noexcept = delete;

        void run();

    private:
        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;

        bool m_running = true;

        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;
        std::shared_ptr<Rte::Network::NetworkModule> m_networkModule;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Network::NetworkModuleServer> m_networkModuleServer;

        std::map<uint32_t, std::unique_ptr<Player>> m_players;
        std::shared_ptr<std::vector<Rte::Entity>> m_entities;
        std::vector<std::unique_ptr<Rte::Entity>> m_projectiles;
        std::map<Rte::Entity, Rte::Network::PackedTexture> m_newEntitiesTextures;

        uint32_t m_currentUid = 0;
};
