#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModule.hpp"
#include "Rte/Network/NetworkModuleServer.hpp"
#include "Rte/Physics/PhysicsModule.hpp"

#include <memory>

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
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Physics::PhysicsModule> m_physicsModule;
        std::shared_ptr<Rte::Network::NetworkModule> m_networkModule;
        std::shared_ptr<Rte::Network::NetworkModuleServer> m_networkModuleServer;
};
