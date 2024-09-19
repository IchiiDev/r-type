#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>

class ClientApp {
    public:
        ClientApp();
        ~ClientApp() = default;

        ClientApp(const ClientApp&) = delete;
        ClientApp& operator=(const ClientApp&) = delete;
        ClientApp(ClientApp&&) noexcept = delete;
        ClientApp& operator=(ClientApp&&) noexcept = delete;

        void run();

    private:
        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;
};
