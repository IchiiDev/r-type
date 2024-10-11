#pragma once

#include "Button.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleClient.hpp"

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


    private:
        void menuLoop();


    private:
        bool m_running = true;

        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;

        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Network::NetworkModuleClient> m_networkModuleClient;


        //////////////////
        // Menu Related //
        //////////////////
        std::vector<std::unique_ptr<Button>> m_menuButtons;
        std::shared_ptr<Rte::Graphic::Texture> m_transparentTexture;
        bool m_switchedOptionsState = false;
        bool m_showOptions = false;
        void createMenuButtons();
        void createOptionsButtons();

        int m_soundVolume = 5;


        //////////////////
        // Game Related //
        //////////////////
        std::vector<Rte::Entity> m_entities;
};
