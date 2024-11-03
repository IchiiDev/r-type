#pragma once

#include "Rte/Ecs/Ecs.hpp"
#include "Rte/Graphic/GraphicModule.hpp"
#include "Rte/Scripting/Components.hpp"
#include "Rte/Scripting/ScriptingModule.hpp"

#include "sol/sol.hpp"

#include <chrono>
#include <memory>
#include <ratio>

namespace Rte::Scripting {

    class ScriptingSystem : public Rte::System {
        public:
            ScriptingSystem() = default;
            ~ScriptingSystem() override = default;

            ScriptingSystem(const ScriptingSystem&) = delete;
            ScriptingSystem& operator=(const ScriptingSystem&) = delete;
            ScriptingSystem(ScriptingSystem&&) = default;
            ScriptingSystem& operator=(ScriptingSystem&&) = default;

            void init(const std::shared_ptr<Rte::Ecs>& ecs, sol::state* lua) {
                m_ecs = ecs;
                m_lua = lua;
            }

            void update() {
                static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
                const std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
                const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
                lastTime = currentTime;

                m_lua->set("deltaTime", deltaTime);

                for (const Rte::Entity& entity : m_entities) {
                    Components::Script& scriptComponent = m_ecs->getComponent<Components::Script>(entity);
                    Rte::BasicComponents::Transform& transformComponent = m_ecs->getComponent<Rte::BasicComponents::Transform>(entity);

                    m_lua->set("Transform", transformComponent);
                    m_lua->script(scriptComponent.code);

                    Rte::BasicComponents::Transform newTransform = (*m_lua)["Transform"];
                    transformComponent.position = newTransform.position;
                    transformComponent.rotation = newTransform.rotation;
                    transformComponent.scale = newTransform.scale;
                }
            }

        private:
            std::shared_ptr<Rte::Ecs> m_ecs;
            sol::state *m_lua = nullptr;
    };

    class ScriptingModuleImpl : public ScriptingModule {
        public:
            // Module methods
            void init(const std::shared_ptr<Ecs>& ecs) override;
            void update() override;


            // Methods
            void setGraphicModule(const std::shared_ptr<Graphic::GraphicModule>& graphicModule) override {
                m_graphicModule = graphicModule;
            }


        private:
            std::shared_ptr<Ecs> m_ecs;
            sol::state m_lua;
            std::shared_ptr<ScriptingSystem> m_scriptingSystem;
            std::shared_ptr<Graphic::GraphicModule> m_graphicModule;
    };

}   // namespace Rte::Scripting
