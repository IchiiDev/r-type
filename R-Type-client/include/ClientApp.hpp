#pragma once

#include "Button.hpp"
#include "Rte/Audio/AudioModule.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Network/NetworkModuleClient.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
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
        std::shared_ptr<Rte::Audio::AudioModule> m_audioModule;
        std::shared_ptr<Rte::Network::NetworkModuleClient> m_networkModuleClient;

        void handleDevConsole();
        void handleConsoleInput();

        //////////////////
        // Menu Related //
        //////////////////
        std::vector<std::unique_ptr<Button>> m_menuButtons;
        bool m_switchedOptionsState = false;
        bool m_showOptions = false;
        bool m_isInMenu = true;
        void createMenuButtons();
        void createOptionsButtons();

        int m_soundVolume = 5;

        /////////////////
        // Dev Console //
        /////////////////

        Rte::Entity m_devConsoleEntity;
        bool m_showDevConsole = false;
        std::string m_devConsoleInput;
        std::vector<std::string> m_devConsoleHistory;
        int m_devConsoleHistoryIndex = -1;

        //////////////////
        // Game Related //
        //////////////////
        std::mutex m_entitiesMutex;
        std::vector<Rte::Entity> m_entities;
};
