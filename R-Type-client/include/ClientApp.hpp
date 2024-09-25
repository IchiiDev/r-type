#pragma once

#include "Button.hpp"

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Graphic/Texture.hpp"
#include "Rte/ModuleManager.hpp"

#include <memory>
#include <vector>

class ClientApp {
    public:
        ClientApp();
        ~ClientApp() = default;

        ClientApp(const ClientApp&) = delete;
        ClientApp& operator=(const ClientApp&) = delete;
        ClientApp(ClientApp&&) noexcept = delete;
        ClientApp& operator=(ClientApp&&) noexcept = delete;

        void run();
        void menuLoop();

    private:
        bool m_running = true;

        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;
        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;

        std::shared_ptr<Rte::Graphic::Texture> m_redTexture;
        std::vector<std::unique_ptr<Button>> m_buttons;
};
