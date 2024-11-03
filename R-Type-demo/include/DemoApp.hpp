#pragma once

#include "Rte/Audio/AudioModule.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Ecs/Types.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Scripting/ScriptingModule.hpp"
#include <vector>

struct Projectile {
    Rte::Entity entity{};
    float distanceDone = 0;
};

class DemoApp {
    public:
        DemoApp();
        ~DemoApp();

        DemoApp(const DemoApp &other) = delete;
        DemoApp &operator=(const DemoApp &other) = delete;
        DemoApp(DemoApp &&other) = delete;
        DemoApp &operator=(DemoApp &&other) = delete;

        void run();


    private:
        bool m_running = true;

        Rte::ModuleManager moduleManager;
        std::shared_ptr<Rte::Ecs> m_ecs;

        std::shared_ptr<Rte::Graphic::GraphicModule> m_graphicModule;
        std::shared_ptr<Rte::Scripting::ScriptingModule> m_scriptingModule;
        std::shared_ptr<Rte::Audio::AudioModule> m_audioModule;

        std::vector<Projectile> m_projectiles;
};
