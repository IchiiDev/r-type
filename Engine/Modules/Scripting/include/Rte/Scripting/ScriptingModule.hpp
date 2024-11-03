#pragma once

#include "Rte/Common.hpp"
#include "Rte/Ecs/Ecs.hpp"
#include "Rte/ModuleManager.hpp"
#include "Rte/Graphic/GraphicModule.hpp"

#include <memory>

namespace Rte::Scripting {

    class ScriptingModule : public IModule {
        public:
            ScriptingModule() = default;
            ~ScriptingModule() override = default;

            ScriptingModule(const ScriptingModule&) = delete;
            ScriptingModule& operator=(const ScriptingModule&) = delete;

            ScriptingModule(ScriptingModule&&) = default;
            ScriptingModule& operator=(ScriptingModule&&) = default;


        public: // Overridden methods
            void init(const std::shared_ptr<Ecs>& ecs) override = 0;
            void update() override = 0;


        public: // Methods
            virtual void setGraphicModule(const std::shared_ptr<Graphic::GraphicModule>& graphicModule) = 0;
    };

}   // namespace Rte::Scripting

extern "C" EXPORT Rte::IModule* createModule();
